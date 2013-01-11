// internal data struct
#include "Nodes.h"
#include "Simulator.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int NbNodes;
Node Nodes;

void LaunchSimulation(int NbRounds, int NbNodes, NodesFct_t NodeFunc){
    int i, j, k, receiver;
    Message msg, msgBis;
    char *buf;
    char* ex_event = NULL;
    size_t bread, nbyte = 0;
    int nb_match;

    // Buffer allocation
    buf = malloc(20 * sizeof(char));
    if(NULL == buf){
        fprintf(stderr,"malloc fail LaunchSimulation.0\n");
        exit(1);
    }

    // Initialization of the nodes
    Nodes = (Node) malloc(NbNodes * sizeof(struct _Node));
    if(NULL == Nodes){
        fprintf(stderr,"malloc fail LaunchSimulation.1\n");
        exit(1);
    }
    for(i = 0; i < NbNodes; i++){
        Nodes[i].sendBuf = CreateFifo();
        Nodes[i].receivBuf = CreateFifo();
        Nodes[i].eventsBuf = CreateFifo();
    }

    // Rounds loop
    for(i = 0; i < NbRounds || NbRounds < 0; i++){
        //Read external events
        printf("Enter the external event for the round %i (format \"start\" or \"n event\")\n", i);
	bread = getline(&ex_event, &nbyte, stdin);
	if(-1 == bread){
            fprintf(stderr, "EOF reached or read failure in getline.\n");
            exit(EXIT_FAILURE);
	}
	// Remove '\n' is it is present at the end of the buffer.
	if(ex_event[bread - 1] == '\n') ex_event[bread - 1] = '\0';
	// Read external event until start is found.
	while(strcmp(ex_event, "start")){
            // Try to parse the input
	    nb_match = sscanf(ex_event, "%i %19s\n", &receiver, buf);
	    if(nb_match < 2){
                fprintf(stderr, "No parse... format is \"n event\"...\n");
		exit(EXIT_FAILURE);
	    }

	    // Creation of a message ??? why ???
	    msg = initMessage(buf, -1, receiver);
            Append(msg,Nodes[receiver].eventsBuf);

	    // Reinit
            printf("Event added, enter an other event or \"start\"\n");
	    bread = getline(&ex_event, &nbyte, stdin);
            if(-1 == bread){
                fprintf(stderr, "EOF reached or read failure in getline.\n");
                exit(EXIT_FAILURE);
            }
	    // Remove '\n' is it is present at the end of the buffer.
	    if(ex_event[bread - 1] == '\n') ex_event[bread - 1] = '\0';
	}

        //Apply NodeFunc for all Nodes
        //With the Message received at the beginning of the round
        //and all the external events
        for(j = 0; j < NbNodes; j++)
            NodeFunc(j, RemoveHead(Nodes[j].receivBuf), Nodes[j].eventsBuf); 

        for(j = 0; j < NbNodes; j++){
            //msg is the older message sended by j 
            //or NULL if no messages 
            msg = RemoveHead(Nodes[j].sendBuf);
            if(NULL != msg){
                if(-1 != msg->receiv){
                    //msg is destinated to one particular node
                    if(msg->receiv >= NbNodes || msg->receiv < -1){
                        fprintf(stderr,"Message send to inexistant receiver ignored\n");
                        break;
                    }
                    Append(msg, Nodes[msg->receiv].receivBuf);
                }else{
                    //msg is a multicast
                    for(k = 0; k < NbNodes; k++){
                        if(k != j){
		            msgBis = copyMessage(msg);
                            Append(msgBis, Nodes[k].receivBuf);
                        }
                    }

		    deleteMessage(msg);
                }
            }
        }
    }

    // Free
    free(buf);
    if(ex_event != NULL) free(ex_event);
}

int Send(Message m){
    if(m!=NULL){
        Append(m,Nodes[m->sender].sendBuf);
        return 0;
    }
    return 1;
}

// Message initialization function.
//   str : content of the message (null-terminated)
//   snd : sender of the message
//   rcv : receiver of the message
// Failures are handled internaly.
// Returns a Message object.
Message initMessage(const char* str, int snd, int rcv){
    Message msg = malloc(sizeof(struct _Message));
    int len = strlen(str) + 1;

    if(NULL == msg){
        fprintf(stderr, "Failed malloc in initMessage.1...\n");
	exit(1);
    }

    msg->msg = malloc(len * sizeof(char));
    if(NULL == msg->msg){
        fprintf(stderr, "Failed malloc in initMessage.2...\n");
	exit(1);
    }

    strncpy(msg->msg, str, len);
    msg->sender = snd;
    msg->receiv = rcv;

    return msg;
}

// Delete a message.
//   msg : the message
// Failures are handled internaly.
void deleteMessage(Message msg){
    if(NULL != msg){
        if(NULL != msg->msg) free(msg->msg);
	free(msg);
    }
}

// Copy a message.
//   msg : the message
// Failures are handled internaly.
// Returns a Message object.
Message copyMessage(Message msg){
    return initMessage(msg->msg, msg->sender, msg->receiv);
}

