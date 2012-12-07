#include"Simulator.h"

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<math.h>

int NbNodes;

//Tree broadcast: log(NbNodes) turn to broadcast
//every node send to its succesors
void TreeBroadcast(int Id, Message_t m, Fifo events){
    Message_t event, msgOut;
    int nTurn;
    //Events Rules
    while((event=RemoveHead(events))!=NULL){
        printf("event received %d %s\n",Id, event->msg); 
        //Read the first event
        if(!strcmp(event->msg,"broadcast")){
            //start a tree broadcast:
            //Iniatialize the message
            nTurn=0;
            while(nTurn<log2(NbNodes)){
                if((msgOut=malloc(sizeof(struct _Message)))==NULL){
                    fprintf(stderr,"malloc fail TreeBroadcast.0\n");
                    exit(1);
                }
                if((msgOut->msg=malloc(sizeof(char)*6))==NULL){
                    fprintf(stderr,"malloc fail TreeBroadcast.1\n");
                    exit(1);
                }
                strcpy(msgOut->msg,"Hello");
                msgOut->sender=Id;
                //at the first step of the tree broadcast, we send a message
                //to our successor
                msgOut->receiv=(int)(pow(2,nTurn)+Id)%NbNodes;
                Send(msgOut);
                nTurn++;
            }
        }
        free(event->msg);
        free(event);
    }
    //Message Rules
    if(m!=NULL){
        printf("%s received by %d from %d\n", m->msg, Id, m->sender);
        //at a step n, a message is sent at a distance 2^n
        //the distance is not exactly the difference between the sender and the
        //receiver id because of the mod N
        if(m->sender<m->receiv){
            nTurn=log2(m->receiv-m->sender);
        }else{
            nTurn=log2(NbNodes-m->sender+m->receiv);
        }
        //this turn is done, let's do the others
        nTurn++;
        //now we can send all the others messages
        while(nTurn<log2(NbNodes)){
            if((msgOut=malloc(sizeof(struct _Message)))==NULL){
                fprintf(stderr,"malloc fail TreeBroadcast.2\n");
                exit(1);
            }
            msgOut->sender=Id;
            msgOut->receiv=((int)(pow(2,nTurn)+Id))%NbNodes;
            //duplicate the message
            if((msgOut->msg=malloc(sizeof(char *)*(strlen(m->msg)+1)))==NULL){
                fprintf(stderr,"malloc fail TreeBroadcast.3\n");
                exit(1);
            }
            msgOut->msg=strcpy(msgOut->msg,m->msg);
            Send(msgOut);
            nTurn++;
        }
        free(m->msg);
        free(m);
    }
}

//Ip broadcast: one node send to everyone on one round
void IPBroadcast(int Id, Message_t m, Fifo events){
    Message_t event, msgOut;
    //Events Rules
    while((event=RemoveHead(events))!=NULL){
        printf("event received %d %s\n",Id, event->msg); 
        //Read the first event
        if(!strcmp(event->msg,"broadcast")){
            //start a basic broadcast:
            //send hello to every nodes
            //Iniatialize the message
            if((msgOut=malloc(sizeof(struct _Message)))==NULL){
                fprintf(stderr,"malloc fail BasicBroadcast.0\n");
                exit(1);
            }
            if((msgOut->msg=malloc(sizeof(char)*6))==NULL){
                fprintf(stderr,"malloc fail BasicBroadcast.1\n");
                exit(1);
            }
            strcpy(msgOut->msg,"Hello");
            msgOut->sender=Id;
            msgOut->receiv=-1;
            Send(msgOut);
        }
        free(event->msg);
        free(event);
    }
    //Message Rules
    if(m!=NULL){
        printf("%s received by %d from %d\n", m->msg, Id, m->sender);
        free(m->msg);
        free(m);
    }
}

//Basic broadcast: N turn to broadcast, all send are done by
//the same node
void BasicBroadcast(int Id, Message_t m, Fifo events){
    Message_t event, msgOut;
    int i;
    //Events Rules
    while((event=RemoveHead(events))!=NULL){
        printf("event received %d %s\n",Id, event->msg); 
        //Read the first event
        if(!strcmp(event->msg,"broadcast")){
            //start a basic broadcast:
            //send hello to every nodes
            for(i=0;i<NbNodes;i++){
                if(i!=Id){
                    //Iniatialize the message
                    if((msgOut=malloc(sizeof(struct _Message)))==NULL){
                        fprintf(stderr,"malloc fail IPBroadcast.0\n");
                        exit(1);
                    }
                    if((msgOut->msg=malloc(sizeof(char)*6))==NULL){
                        fprintf(stderr,"malloc fail IPBroadcast.1\n");
                        exit(1);
                    }
                    strcpy(msgOut->msg,"Hello");
                    msgOut->sender=Id;
                    msgOut->receiv=i;
                    Send(msgOut);
                }
            }
        }
        free(event->msg);
        free(event);
    }
    //Message Rules
    if(m!=NULL){
        printf("%s received by %d from %d\n", m->msg, Id, m->sender);
        free(m->msg);
        free(m);
    }
}
int main (int argc, char **argv)
{
    NodesFct_t f;
    if(argc!=4){
        fprintf(stderr,"usage %s Nround Nodes function\n The available function are basic, tree, ip\n", argv[0]);
        exit(1);
    }
    if(!strcmp(argv[3],"basic")){
        f=BasicBroadcast;
    }else if(!strcmp(argv[3],"tree")){
        f=TreeBroadcast;
    }else if(!strcmp(argv[3],"ip")){
        f=IPBroadcast;
    }else{
        fprintf(stderr,"unknown function %s\n", argv[3]);
        exit(1);
    }
    NbNodes=atoi(argv[2]);
    LaunchSimulation(atoi(argv[1]),NbNodes,f);
    return 0;
}
