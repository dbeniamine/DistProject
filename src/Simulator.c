// internal data struct
#include "Nodes.h"
#include "Simulator.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int NbNodes;
Node Nodes;

/*precond:
 *buf contains a string on the format
 *integer text\0
 *post cond:
 *buf contains text \0
 *num contains the integer
 */

int split(char buf[], int *num){
    int i=0, j=0;
    while(buf[i]!=' ' && buf[i]!='\n' && buf[i]!='\0'){
        i++;
    }
    if(buf[i]=='\0'){
        //error no space char
        return -1;
    }
    buf[i]='\0';
    *num=atoi(buf);
    i++;
    //move th string
    while(buf[i]!='\0'){
        buf[j]=buf[i];
        j++;
        i++;
    }
    buf[j]='\0';
    return 0;
}

void LaunchSimulation(int NbRounds, int NbProcess, NodesFct_t NodeFunc){
    int i,j,k, receiver;
    Message_t msg, msgBis;
    char *buf;
    //initialization
    NbNodes=NbProcess;

    //Nodes creation
    if((Nodes=(Node )malloc(NbNodes*sizeof(struct _Node)))==NULL){
        fprintf(stderr,"malloc fail LaunchSimulation.0\n");
        exit(1);
    }
    for(i=0;i<NbNodes;i++){
        Nodes[i].sendBuf=CreateFifo();
        Nodes[i].receivBuf=CreateFifo();
        Nodes[i].eventsBuf=CreateFifo();
    }

    //Do rounds
    i=0;
    while( i<NbRounds || NbRounds<0){
        //Read external events
        printf("Enter the external event for the round %d, The event format is numProcess event\\n, or \"start\" to start the round\n", i);
        if((buf=malloc(sizeof(char)*200))==NULL){
            fprintf(stderr,"malloc fail LaunchSimulation.1\n");
            exit(1);
        }       
        scanf("%[^\n]200", buf);
        if(getchar()!='\n'){
            fprintf(stderr,"internal error LaunchSimulation.0\n");
            exit(1);
        }
        while(strcmp(buf,"start")){
            //add external event
            if(split(buf, &receiver)<0){
                printf("error parsing event %s, retry\n", buf);
            }else{
                if((msg=malloc(sizeof(struct _Message)))==NULL){
                    fprintf(stderr,"malloc fail LaunchSimulation.2\n");
                    exit(1); 
                }
                msg->sender=-1;
                msg->receiv=receiver;
                msg->msg=buf;
                Append(msg,Nodes[receiver].eventsBuf);
            }
            printf("Event added, enter an other event or \"start\"\n");
            if((buf=malloc(sizeof(char)*200))==NULL){
                fprintf(stderr,"malloc fail LaunchSimulation.3\n");
                exit(1);
            }
            scanf("%[^\n]200", buf);
            if(getchar()!='\n'){
                fprintf(stderr,"internal error LaunchSimulation.1\n");
                exit(1);
            }
        } 
        free(buf); 
        for(j=0;j<NbNodes;j++){
            //Apply NodeFunc for all Nodes
            //With the Message received at the beginning of the round
            //and all the external events
            NodeFunc(j,RemoveHead(Nodes[j].receivBuf),Nodes[j].eventsBuf); 
        }
        for(j=0; j<NbNodes;j++){
            //msg is the older message sended by j 
            //or NULL if no messages 
            msg=RemoveHead(Nodes[j].sendBuf);
            if(msg!=NULL){
                if(msg->receiv!=-1){
                    //msg is destinated to one particular node
                    if(msg->receiv>=NbNodes || msg->receiv<-1){
                        fprintf(stderr,"Message send to inexistant receiver ignored\n");
                        break;
                    }
                    Append(msg, Nodes[msg->receiv].receivBuf);
                }else{
                    //msg is a multicast
                    for(k=0;k<NbNodes;k++){
                        if(k!=j){
                            //duplicate the original message
                            if((msgBis=malloc(sizeof(struct _Message)))==NULL){
                                fprintf(stderr,"malloc fail LaunchSimulation.4\n");
                                exit(1);
                            }
                            if((msgBis->msg=malloc(sizeof(char)*(strlen(msg->msg)+1)))==NULL){
                                fprintf(stderr,"malloc fail LaunchSimulation.5\n");
                                exit(1);
                           }
                            msgBis->msg=strcpy(msgBis->msg,msg->msg);
                            msgBis->sender=msg->sender;
                            msgBis->receiv=msg->receiv;
                            Append(msgBis, Nodes[k].receivBuf);
                        }
                    }
                    free(msg->msg);
                    free(msg);
                }
            }
        }
        i++;
    }
}




int Send(Message_t m){
    if(m!=NULL){
        Append(m,Nodes[m->sender].sendBuf);
        return 0;
    }
    return 1;
}

