/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * This file contains the implementation of the broadcast protocols.        *
 * Author: David Beniamine and Rodolphe Lepigre                             *
 ****************************************************************************/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "Simulator.h"
#include "Broadcast.h"
#include "SortedList.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))

int maxArgSize=200;     //arbitratry maximum size for a broadcast argument
int intToStringSize=11; //2^32 = 4294967296 =10 char +1 for '\0' => 11 char to convert one int to string

/*
 * Standardized deliver function for all broadcast protocols.
 * m    : the message to deliver.
 * id   : the node delivering the message.
 */
void deliver(Message m, int id){
    printf("Delivered: %s by %i, sender: %i, origin: %i\n", m->msg,
            id, m->sender, m->origin);
}

/*
 * Basic broadcast. The broadcast takes N turns, all sends are done by the
 * same node (one at each turn).
 * Note that like every functions defined here, they have the type NodesFct.
 */
void BasicBroadcast(int id, Message m){
    char* event;
    Message msgOut;
    int i;

    //Events Rules
    while((event = getNextExternalEvent(id)) != NULL){
        printf("event received %d %s\n", id, event); 
        //Read the first event
        if(!strcmp(event, "broadcast")){
            //start a basic broadcast:
            //send hello to every nodes
            for(i = 0; i < getNbNodes(); i++){
                msgOut = initMessage("Hello\0", id, id, i);
                if(i != id){
                    Send(msgOut);
                } else {
                    deliver(msgOut,id);
                    deleteMessage(msgOut);
                }
            }
        }
        free(event);
    }

    //Message Rules
    if(NULL != m){
        deliver(m,id);
        deleteMessage(m);
    }
}

/*
 * Tree broadcast. This protocol needs log(NbNodes) turn to broadcast.
 * Every node sends to its succesors.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void TreeBroadcast(int id, Message m){
    char* event;
    Message msgOut;
    int nTurn;

    //Events Rules
    while((event = getNextExternalEvent(id)) != NULL){
        printf("Event received %d %s\n", id, event); 
        //Read the first event
        if(!strcmp(event, "broadcast")){
            //start a tree broadcast:
            //Iniatialize the message
            for(nTurn = 0; nTurn < log2(getNbNodes()); nTurn++){
                //at the first step of the tree broadcast, we send a message
                //to our successor
                msgOut = initMessage("Hello\0", id, id, (int)(pow(2,nTurn)+id)%getNbNodes());
                Send(msgOut);
            }
            // Deliver the message localy
            msgOut = initMessage("Hello\0", id, id, id);
            deliver(msgOut,id);
            deleteMessage(msgOut);
        }
        free(event);
    }

    //Message Rules
    if(NULL != m){
        deliver(m,id);
        //at a step n, a message is sent at a distance 2^n
        //the distance is not exactly the difference between the sender and the
        //receiver id because of the mod N
        if(m->sender < m->receiv)
            nTurn=log2(m->receiv-m->sender);
        else
            nTurn=log2(getNbNodes()-m->sender+m->receiv);
        //this turn is done, let's do the others
        //now we can send all the others messages
        for(nTurn++; nTurn<log2(getNbNodes()); nTurn++){
            msgOut = initMessage(m->msg, m->origin, id, ((int)(pow(2,nTurn)+id))%getNbNodes());
            Send(msgOut);
        }
        deleteMessage(m);
    }
}

/*
 * Pipeline broadcast. This protocol takes 1 turn to broadcast, the node
 * performing the broadcast sends the message to the next note, which in turn
 * transmits the message to its neighbour and so on.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void PipelineBroadcast(int id, Message m){
    char* event;
    Message msg, fwd;
    int neighbor;

    // Event Rules
    while((event = getNextExternalEvent(id)) != NULL){
        printf("event received %i %s\n",id, event); 
        // Read the first event
        if(0 == strcmp(event, "broadcast")){
            neighbor = (id + 1) % getNbNodes();
            msg = initMessage("Hello\0", id, id, neighbor);
            Send(msg);
            deliver(msg,id);
        }
        free(event);
    }

    // Message Rules
    if(NULL != m){
        deliver(m, id);

        // Forward the message if need be
        neighbor = (id + 1) % getNbNodes();
        if(neighbor != m->origin){
            fwd = initMessage(m->msg, m->origin, id, neighbor);
            Send(fwd);
        }

        // Free the local message
        deleteMessage(m);
    }
}

/*
 * Total Order Broadcast with good latency.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void TOBLatencyBroadcast(int id, Message m){
    char* event;
    Message msgOut;
    int i;

    //Events Rules
    while((event = getNextExternalEvent(id)) != NULL){
        printf("Event received %d %s\n", id, event); 
        //Read the first event
        if(!strcmp(event, "broadcast")){
            if(0 == id){
                // The broadcast is your own, deliver
                msgOut = initMessage("Hello\0", id, id, id);
                deliver(msgOut, id);
                deleteMessage(msgOut);
                // Pass the message to your childs
                for(i = 1; i < getNbNodes(); i *= 2){
                    msgOut = initMessage("Hello\0", id, id, i);
                    Send(msgOut);
                }
            } else {
                // Send the message to process 0
                msgOut = initMessage("Hello\0", id, id, 0);
                Send(msgOut);
            }
        }
        free(event);
    }

    // Message Rules
    if(NULL != m){
        if(0 == id)
            printf("0 receives a message to relay from %i\n", m->sender);

        deliver(m, id);

        for(i = 2 * id + 1; i < getNbNodes(); i *= 2){
            msgOut = initMessage(m->msg, m->origin, id, i);
            Send(msgOut);
        }

        deleteMessage(m);
    }
}

/*
 * Internal data structures and methodes 
 * needed by the
 * Total ordered broadcast with good latency
 */
typedef struct _PipleineAckData_t{
    int clock;          //Logical clock
    int next;           //Next node in the pipeline
    //int pred;           //Previous node in the pipeline
    SortedList pending; //Sorted list of pending messages
}*PipelineAckData_t;

/*
 * Message with a clock and counter for
 * acknowledgment
 */
typedef struct _NumberedMessage{
    int clk;            //timestamp of the message
    int origin;        //the origin of the broadcast
    Message m;          //the message
}*NumberedMessage;

/*
 * Comparator for NumberedMessage
 * m1 > m2 <=>
 * m1->clk > m2->clk or 
 * m1->clk == m2->clk and m1->sender < m2->sender
 */

int NumberedMsgComp(void *e1, void *e2){
    NumberedMessage m1=(NumberedMessage)e1, m2=(NumberedMessage)e2;
    if(m1->clk==m2->clk){
        return m1->origin-m2->origin;
    }
    return m1->clk-m2->clk;
}

/*
 * Extract the first int from a string at the format
 * int1 string
 * i1 : the reference to the first int
 */
void extractInt(char * str, int *i1){
    char *strclone, *tmp;
    if((strclone=malloc((strlen(str)+1)))==NULL){
        fprintf(stderr,"malloc fail at extractInt\n");
        exit(EXIT_FAILURE);
    }
    //we work on a copy of thestring 
    strclone=strcpy(strclone,str);
    //extraction of the first int
    tmp=strchr(strclone,' ');
    *tmp='\0';
    tmp++;
    *i1=atoi(strclone);
    //cleaning data
    free(strclone);
}

/*
 * create an ack at the format: clk initiator ack
 * clk      : the timestamp of the message to acknowledge
 * init     : the original sender of the message to acknowledge
 * sender   : the node who send the ack
 * receiver : the node ho will receive the ack
 * returns the ack
 */
Message Ack(int clk, int init, int sender, int receiver){
    char *msgTxt;
    Message m;
    if((msgTxt=malloc(sizeof(char)*(intToStringSize+3/*ack*/)))==NULL){
        fprintf(stderr,"malloc fail at Ack\n");
        exit(EXIT_FAILURE);
    }
    sprintf(msgTxt, "%d ack", clk);
    m=initMessage(msgTxt,init,sender,receiver); 
    free(msgTxt);
    return m;
}

/*
 * Total Order Broadcast with good throughput.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void TOBThroughputBroadcast(int id, Message m){

    PipelineAckData_t data;
    int argSize, ackOrigin;
    char *event, *eventArg, *msgTxt;
    Message mOut;
    NumberedMessage NumMsg, temp;

    if((data=(PipelineAckData_t)getData(id))==NULL){
        //Initialization

        if((data=malloc(sizeof(struct _PipleineAckData_t)))==NULL){
            fprintf(stderr,"malloc fail at TOBThroughputBroadcast\n");
            exit(EXIT_FAILURE);
        }

        data->clock=0;
        data->next=(id+1)%getNbNodes();
        //        data->pred=(id==0)?getNbNodes()-1:id-1;
        data->pending=newSortedList(NumberedMsgComp);
        setData(id,data);
    }

    if(m==NULL){
        // Event Rules
        if((event = getNextExternalEvent(id)) != NULL){
            printf("event received %i %s\n",id, event); 
            // Read the first event
            if(event==strstr(event, "broadcast")){
                //there is an event : increment the clock
                data->clock++;
                //event is someting like broadcast <string>
                if((eventArg=malloc(sizeof(char)*maxArgSize))==NULL){
                    fprintf(stderr,"malloc fail at TOBThroughputBroadcast\n");
                    exit(EXIT_FAILURE);
                }
                if(1>sscanf(event, "broadcast %s", eventArg)){
                    sscanf("hello","%s",eventArg);
                }
                //eventArg is either the string to broadcast, or "hello"
                argSize=strlen(eventArg);
                if((msgTxt=malloc((intToStringSize+1+argSize)*sizeof(char)))==NULL){
                    fprintf(stderr,"malloc fail at TOBThroughputBroadcast\n");
                    exit(EXIT_FAILURE);
                }
                sprintf(msgTxt, "%d ",data->clock);
                strncat(msgTxt, eventArg, argSize); 
                //Now the message text have the format: clk <arg>
                if((NumMsg=malloc(sizeof(struct _NumberedMessage)))==NULL){
                    fprintf(stderr,"malloc fail at TOBThroughputBroadcast\n");
                    exit(EXIT_FAILURE);
                }
                NumMsg->clk=data->clock;
                //waiting for 1 ack
                //creating the actual message
                NumMsg->m=initMessage(msgTxt,id,id,data->next);
                free(msgTxt);
                NumMsg->origin=id;
                if(data->next==id){
                    //we are the only process of the system
                    deliver(NumMsg->m,id);
                    free(NumMsg->m->msg);
                    free(NumMsg->m);
                    free(NumMsg);
                }else{
                    //and send it
                    Send(copyMessage(NumMsg->m));
                    //Finally store the numbered msg in the pending list
                    AddSorted(NumMsg,data->pending);
                }
            }
            free(event);
        }

    }else{
        // Message Rules
        if((NumMsg=malloc(sizeof(struct _NumberedMessage)))==NULL){
            fprintf(stderr,"malloc fail at TOBThroughputBroadcast\n");
            exit(EXIT_FAILURE);
        }

        //this is a new message at the format:
        //clock <text> where text is either ack or the text of the
        //message
        //We have to parse the clock of the broadcast
        extractInt(m->msg, &(NumMsg->clk));
        NumMsg->origin=m->origin;
        //don't forget to update the clock
        data->clock=MAX(NumMsg->clk,data->clock)+1;

        if(strstr(m->msg,"ack")==NULL){
            printf("%s received by %d from %d but not delivered yet\n", m->msg, id, m->sender);
            //m is an actual message
            NumMsg->m=m;
            if(m->origin!=data->next){
                //We need to forward the message
                mOut=initMessage(m->msg,m->origin,id,data->next);
                Send(mOut);
                AddSorted(NumMsg,data->pending);
            }else{
                AddSorted(NumMsg,data->pending);
                //we deliver all the older messages
                mOut=NULL;
                while((temp=getFirst(data->pending))!=NULL && NumMsg!=NULL && NumberedMsgComp(temp,NumMsg)<=0){
                    //we remove the message from the pending list
                    RemoveFirst(data->pending);
                    if(!NumberedMsgComp(temp,NumMsg)){
                        //to ensure we stop after that
                        NumMsg=NULL;
                    }
                    //prepare an ack
                    if(temp->origin!=id){
                        if(mOut!=NULL){
                            free(mOut);
                        }
                        mOut=Ack(temp->clk,temp->origin,id,data->next);
                    }
                    //deliver
                    deliver(temp->m,id);
                    //we can free the message
                    free(temp->m->msg);
                    free(temp->m);
                    free(temp);
                }
                //if we have managed to deliver a message
                //we acknowleged the older message delvered
                if(mOut){
                    Send(mOut);
                }
            }

        }else{
            //this is an ack
            printf("%s received by %d from %d origin %d\n", m->msg, id, m->sender, m->origin);
            //we deliver all the older messages
            mOut=NULL;
            while((temp=getFirst(data->pending))!=NULL && NumMsg!=NULL && NumberedMsgComp(temp,NumMsg)<=0){
                //we remove the message from the pending list
                RemoveFirst(data->pending);
                //prepare an ack
                ackOrigin=((temp->origin+getNbNodes()-1)%getNbNodes());
                if(ackOrigin!=data->next){
                    if(mOut!=NULL){
                        free(mOut);
                    }
                    mOut=Ack(temp->clk,temp->origin,id,data->next);
                }
                //deliver
                deliver(temp->m,id);
                //we can free the message
                free(temp->m->msg);
                free(temp->m);
                free(temp);
            }
            //if we have managed to deliver a message
            //we acknowleged the older message delvered
            ackOrigin=((NumMsg->origin+getNbNodes()-1)%getNbNodes());
            if(mOut){
                Send(mOut);
            }else if(Size(data->pending)==0 && ackOrigin!=data->next){
                //we haven't be able to deliver a message because our pending
                //list is empty, but the message may be blocked in someon else
                //queue so we forward the ack
                Send(Ack(NumMsg->clk,NumMsg->origin,id,data->next));
            }
            // Free the local message
            free(m->msg);
            free(m);
        }
    }

}

/*
 * Total Order Broadcast with good throughput.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void TOBThroughputRodBroadcast(int id, Message m){
    char* event;
    Message msgOut;

    //Events Rules
    while((event = getNextExternalEvent(id)) != NULL){
        printf("Event received %d %s\n", id, event); 
        //Read the first event
        if(!strcmp(event, "broadcast")){
            if(0 == id){
                // The broadcast is your own, deliver
                msgOut = initMessage("Hello\0", id, id, id);
                deliver(msgOut, id);
                deleteMessage(msgOut);
                // Pass the message to your successor

                if(1 != getNbNodes()){
                    msgOut = initMessage("Hello\0", 0, 0, 1);
                    Send(msgOut);
                }
            } else {
                // Send the message to process 0
                msgOut = initMessage("Hello\0", id, id, 0);
                Send(msgOut);
            }
        }
        free(event);
    }

    // Message Rules
    if(NULL != m){
        if(0 == id)
            printf("0 receives a message to relay from %i\n", m->sender);

        deliver(m, id);

        // If not the last of the pipeline...
	if(id != getNbNodes() - 1){
            // Transfer the message to you successor
            msgOut = initMessage(m->msg, m->origin, id, id + 1 % getNbNodes());
            Send(msgOut);
	}

        deleteMessage(m);
    }
}
