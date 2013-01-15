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

#define MAX(a,b) (a) > (b) ? (a) : (b)

int maxArgSize=200;     //arbitratry maximum size for a broadcast argument
int intToStringSize=11; //2^32 = 4294967296 =10 char +1 for '\0' => 11 char to convert one int to string

/*
 * Standardized deliver function for all
 * broadcasts
 * m    : the message delivered
 * id   : the node which delivers the message
 */
void deliver(Message m, int id){
    printf("message delivered : %s sender : %d on node %d\n", m->msg,m->sender ,id);
}

/*
 * Ip broadcast. A node can send to every other node in one round.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void IPBroadcast(int id, Message m){
    char* event;
    Message msgOut;

    // Events Rules
    while((event = getNextExternalEvent(id)) != NULL){
        printf("Event received %d %s\n", id, event); 
        //Read the first event
        if(!strcmp(event, "broadcast")){
            //start a basic broadcast:
            //send hello to every nodes
            //Iniatialize the message
            msgOut = initMessage("Hello\0", id, -1);
            Send(msgOut);
        }
        free(event);
    }

    //Message Rules
    if(NULL != m){
        deliver(m,id);
        free(m->msg);
        free(m);
    }
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
                if(i != id){
                    msgOut = initMessage("Hello\0", id, i);
                    Send(msgOut);
                }
            }
        }
        free(event);
    }

    //Message Rules
    if(NULL != m){
        deliver(m,id);
        free(m->msg);
        free(m);
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
                msgOut = initMessage("Hello\0", id, (int)(pow(2,nTurn)+id)%getNbNodes());
                Send(msgOut);
            }
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
            msgOut = initMessage(m->msg, id, ((int)(pow(2,nTurn)+id))%getNbNodes());
            Send(msgOut);
        }
        free(m->msg);
        free(m);
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
    int neighbor, broadcaster;
    char* content;

    content = malloc(128 * sizeof(char));
    if(NULL == content){
        fprintf(stderr, "Failed malloc in PipelineBroadcast...\n");
        exit(EXIT_FAILURE);
    }

    // Event Rules
    while((event = getNextExternalEvent(id)) != NULL){
        printf("event received %i %s\n",id, event); 
        // Read the first event
        if(0 == strcmp(event, "broadcast")){
            neighbor = (id + 1) % getNbNodes();
            sprintf(content, "from %i : Hello", id);
            msg = initMessage(content, id, neighbor);
            Send(msg);
        }
        free(event);
    }

    // Message Rules
    if(NULL != m){
        if(1 < sscanf(m->msg, "from %i : %s", &broadcaster, content)){
            // Print content and data
            printf("%s received by %i from %i (broadcasted by %i)\n",
                    content, id, m->sender, broadcaster);

            // Forward the message if need be
            neighbor = (id + 1) % getNbNodes();
            if(neighbor != broadcaster){
                fwd = initMessage(m->msg, id, neighbor);
                Send(fwd);
            }

            // Free the local message
            free(m->msg);
            free(m);
        }
    }

    free(content);
}

/*
 * Total Order Broadcast with good latency.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void TOBLatencyBroadcast(int id, Message m){
    char* event;
    Message msgOut;
    char* msgContent;
    int i;

    //Events Rules
    while((event = getNextExternalEvent(id)) != NULL){
        printf("Event received %d %s\n", id, event); 
        //Read the first event
        if(!strcmp(event, "broadcast")){
            if(0 == id){
                // The broadcast is your own, deliver
                printf("Hello from 0! received by 0 from 0\n");
                // Pass the message to your childs
                for(i = 1; i < getNbNodes(); i *= 2){
                    msgOut = initMessage("Hello from 0!\0", id, i);
                    Send(msgOut);
                }
            } else {
                // Send the message to process 0
                msgContent = malloc(64 * sizeof(char));
                snprintf(msgContent, 64, "Hello from %i!", id);
                msgOut = initMessage(msgContent, id, 0);
                Send(msgOut);
            }
        }
        free(event);
    }

    // Message Rules
    if(NULL != m){
        printf("%s received by %d from %d\n", m->msg, id, m->sender);

        for(i = 2 * id + 1; i < getNbNodes(); i *= 2){
            msgOut = initMessage(m->msg, id, i);
            Send(msgOut);
        }

        free(m->msg);
        free(m);
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
    int pred;           //Previous node in the pipeline
    SortedList pending; //Sorted list of pending messages
}*PipelineAckData_t;

/*
 * Message with a clock and counter for
 * acknowledgment
 */
typedef struct _NumberedMessage{
    int clk;            //timestamp of the message
    int creator;        //Creator of the message
    int waitForNbAck;   //number of ack that we are stil waiting for
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
        return m1->creator-m2->creator;
    }
    return m1->clk-m2->clk;
}

/*
 * Extract the two first int from a string at the format
 * int1 int2 string
 * i1 : the reference to the first int
 * i2 the reference to the second int
 */
void extractTwoInts(char * str, int *i1, int *i2){
    char *strclone, *tmp, *tmp1;
    if((strclone=malloc((strlen(str)+1)))==NULL){
        fprintf(stderr,"malloc fail at extractTwoInts\n");
        exit(EXIT_FAILURE);
    }
    //we work on a copy of thestring 
    strclone=strcpy(strclone,str);
    //extraction of the first int
    tmp=strchr(strclone,' ');
    *tmp='\0';
    tmp++;
    *i1=atoi(strclone);
    //extraction of the second
    tmp1=strchr(tmp,' ');
    *tmp1='\0';
    *i2=atoi(tmp);
    free(strclone);
}

/*
 * Send an ack at the format: clk initiator ack
 * clk      : the timestamp of the message to acknowledge
 * init     : the original sender of the message to acknowledge
 * sender   : the node who send the ack
 * receiver : the node ho will receive the ack
 */
void sendAck(int clk, int init, int sender, int receiver){
    char *msgTxt;
    Message m;
    if((msgTxt=malloc(sizeof(char)*(2*intToStringSize+3/*ack*/)))==NULL){
        fprintf(stderr,"malloc fail at sendAck\n");
        exit(EXIT_FAILURE);
    }
    sprintf(msgTxt, "%d %d ack", clk, init);
    m=initMessage(msgTxt,sender,receiver); 
    Send(m);
}

/*
 * Total Order Broadcast with good throughput.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void TOBThroughputBroadcast(int id, Message m){

    PipelineAckData_t data;
    int argSize;
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
        data->pred=(id==0)?getNbNodes()-1:id-1;
        data->pending=newSortedList(NumberedMsgComp);
        setData(id,data);
    }

    // Event Rules
    while((event = getNextExternalEvent(id)) != NULL){
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
            if((msgTxt=malloc((2*intToStringSize+1+argSize)*sizeof(char)))==NULL){
                fprintf(stderr,"malloc fail at TOBThroughputBroadcast\n");
                exit(EXIT_FAILURE);
            }
            sprintf(msgTxt, "%d %d ",data->clock, id);
            strncat(msgTxt, eventArg, argSize); 
            free(eventArg); //not needed anymore
            //Now the message text have the format: clk creator <arg>
            if((NumMsg=malloc(sizeof(struct _NumberedMessage)))==NULL){
                fprintf(stderr,"malloc fail at TOBThroughputBroadcast\n");
                exit(EXIT_FAILURE);
            }
            NumMsg->clk=data->clock;
            //waiting for 1 ack
            NumMsg->waitForNbAck=1;
            //we are the creator of this message
            NumMsg->creator=id;
            //creating the actual message
            NumMsg->m=initMessage(msgTxt,id,data->next);
            //and send it
            Send(copyMessage(NumMsg->m));
            //Finally store the numbered msg in the pending list
            AddSorted(NumMsg,data->pending);
        }
        free(event);
    }

    // Message Rules
    if(NULL != m){
        if((NumMsg=malloc(sizeof(struct _NumberedMessage)))==NULL){
            fprintf(stderr,"malloc fail at TOBThroughputBroadcast\n");
            exit(EXIT_FAILURE);
        }

        //this is a new message at the format:
        //clock creator <text> where text is either ack or the text of the
        //message
        //We have to parse the clock and the creator of the broadcast
        extractTwoInts(m->msg, &(NumMsg->clk),&(NumMsg->creator));
        //don't forget to update the clock
        data->clock=MAX(NumMsg->clk,data->clock)+1;

        if(strstr(m->msg,"ack")==NULL){
            printf("%s received by %d from %d but not delivered yet\n", m->msg, id, m->sender);
            //m is an actual message
            NumMsg->m=m;
            if(NumMsg->creator!=data->next){
                //We need to forward the message
                mOut=initMessage(m->msg,id,data->next);
                NumMsg->waitForNbAck=1; //we have to wait for an ack
                Send(mOut);
                AddSorted(NumMsg,data->pending);
            }else{
                //we are the end of the pipeline 
                NumMsg->waitForNbAck=0;
                AddSorted(NumMsg, data->pending);
                //we send an ack as soon as the message is received
                sendAck(NumMsg->clk,NumMsg->creator,id,data->pred);
                if(!NumberedMsgComp(getFirst(data->pending),NumMsg)){
                    //don't forget to remove the message from the pending list
                    RemoveFirst(data->pending);
                    //And we delivers it message !
                    deliver(NumMsg->m,id);
                    free(NumMsg->m->msg);
                    free(NumMsg->m);
                    free(NumMsg);

                }
            }

        }else{
            //this is an ack
            if((temp=getFirst(data->pending))!=NULL && !NumberedMsgComp(NumMsg,temp)){
                printf("%s received by %d from %d \n", m->msg, id, m->sender);
                free(NumMsg);   //NumMsg was a temporary variable, we free it and we work
                //with the messages on the pending
                //we will try to deliver and ack as much message as possible
                NumMsg=temp;
                if(NumMsg->creator!=id){
                    sendAck(NumMsg->clk,NumMsg->creator,id,data->pred);
                }
                NumMsg->waitForNbAck=0;
                while(NumMsg!=NULL && NumMsg->waitForNbAck==0){
                    //we remove the message from the pending list
                    RemoveFirst(data->pending);
                    //deliver
                    deliver(NumMsg->m,id);
                    //we can free the message
                    free(NumMsg->m->msg);
                    free(NumMsg->m);
                    free(NumMsg);
                    //and we look at the new head of the list
                    NumMsg=getFirst(data->pending);
                }
            }else{
                //we can't deliver the message yet
                //we just note that we have received the ack and we forward it
                if((temp=Remove(NumMsg,data->pending))==NULL){
                    fprintf(stderr,"duplicated ack %s on %d from %d \n",  m->msg, id, m->sender);
                    exit(EXIT_FAILURE);
                }else{
                    printf("%s received by %d from %d \n", m->msg, id, m->sender);
                    //free(NumMsg);
                    temp->waitForNbAck=0;
                    AddSorted(temp,data->pending);
                    if(temp->creator!=id){
                        sendAck(temp->clk,temp->creator,id,data->pred);
                    }
                }
            }
            // Free the local message
            free(m->msg);
            free(m);
        }
    }

}
