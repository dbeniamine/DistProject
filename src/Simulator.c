/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * This file contains the implementation of the core functions of the       *
 * simulator.                                                               *
 * Author: David Beniamine                                                  *
 ****************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "Simulator.h"
#include "Message.h"
#include "Fifo.h"

/*
 * Internal structure representing the system.
 * The last field holds a function to be execuded by the nodes.
 */
typedef struct _System{
  Node* nodes;      // The nodes (process).
  int nb_nodes;     // Number of nodes.
  int nb_rounds;    // Number of rounds to simulate.
  void(*fun)(int id, Message m); // Function
}*System;


static System sys;

/*
 * Initializes a system.
 * nb_nodes  : number of nodes to create.
 * nb_rounds : number of rounds to run.
 * fun       : function to execute by the nodes.
 */
void initSystem(int nb_nodes, int nb_rounds, NodesFct fun){
  int i;
 
  sys = malloc(sizeof(struct _System));
  if(NULL == sys){
    fprintf(stderr,"Malloc error in initSystem...\n");
    exit(1);
  }

  sys->nb_nodes = nb_nodes;
  sys->nb_rounds = nb_rounds;
  sys->fun = fun;

  sys->nodes = (Node*) malloc(nb_nodes * sizeof(struct _Node));

  for(i = 0; i < nb_nodes; i++){
    sys->nodes[i].receiv = NULL;
    sys->nodes[i].sendBuf = CreateFifo();
    sys->nodes[i].eventsBuf = CreateFifo();
    sys->nodes[i].data=NULL;
  }
}

/*
 * Delete a system.
 */
void deleteSystem(){
  int i;

  if(NULL != sys){
    if(NULL != sys->nodes){
      for(i = 0; i < sys->nb_nodes; i++){
        DeleteFifo(sys->nodes[i].sendBuf);
        DeleteFifo(sys->nodes[i].eventsBuf);
        if(NULL != sys->nodes[i].receiv)
          free(sys->nodes[i].receiv);
	if(NULL != sys->nodes[i].data)
          free(sys->nodes[i].data);
      }

      free(sys->nodes);
    }

    free(sys);
  }
}

/*
 * Function used by a node to send a message m which must be correcly
 * initialized.
 * m   : the message to send.
 * Returns 0 on success.
 * Returns 1 on failure (m is NULL)
 */
int Send(Message m){
  if(NULL != m){
    Append(m,sys->nodes[m->sender].sendBuf);
    return 0;
  }

  return 1;
}

/*
 * Function used by a node to get its next externel event.
 * id : the node id.
 * Returns the string correponding to the next event if any or NULL.
 */
char *getNextExternalEvent(int id){
  return RemoveHead(sys->nodes[id].eventsBuf);
}
 
/*
 * Function used by a node to get the total number of nodes in the system
 * Returns the number of nodes in the system.
 */
int getNbNodes(){
  return sys->nb_nodes;
}

/*
 * Read external events on the standard input.
 * Events are added directly to the nodes event Fifo in the system.
 * External event format:
 *   start
 *   n event
 */
void readExternalEvents(){
  char* ex_event = NULL;
  size_t bread, nbyte = 0;
  char* msg;
  int receiver;
  int nb_match;
  char *buf;

  // Buffer allocation
  buf = malloc(20 * sizeof(char));
  if(NULL == buf){
    fprintf(stderr,"Malloc error in readExternalEvents...\n");
    exit(1);
  }

  // Read a line
  printf("Enter an external events (format \"start\" or \"n event\")\n");
  bread = getline(&ex_event, &nbyte, stdin);
  if(-1 == bread){
    fprintf(stderr, "EOF reached or read failure in readExternalEvents.\n");
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
    msg = malloc(strlen(buf) * sizeof(char));
    if(NULL == msg){
      fprintf(stderr, "Malloc error in readExternalEvents...\n");
      exit(EXIT_FAILURE);
    }

    strncpy(msg, buf, strlen(buf));
    if(receiver>=0 && receiver<sys->nb_nodes){
        Append(msg,sys->nodes[receiver].eventsBuf);
    }

    // Reinit
    printf("Event added, enter an other event or \"start\"\n");
    bread = getline(&ex_event, &nbyte, stdin);
    if(-1 == bread){
      fprintf(stderr, "EOF reached or read failure in readExternalEvents.\n");
      exit(EXIT_FAILURE);
    }
    // Remove '\n' is it is present at the end of the buffer.
    if(ex_event[bread - 1] == '\n') ex_event[bread - 1] = '\0';
  }

  free(buf);
}

/*
 * Function used by a node to create its own 
 * persistent data set
 * id : the node id
 * data : the new data set
 * Returns the previous data set, or NULL
 */
void *setData(int id, void *data){
    void *old=sys->nodes[id].data;
    sys->nodes[id].data=data;
    return old;
}

/*
 * Function used by a node to acces its own
 * persistent data set
 * id : the node id
 * Returns the data set, or NULL
 */
void *getData(int id){
    return sys->nodes[id].data;
}



/*
 * Core simulation function.
 * The external events are read before each round. They are handled later by
 * the node function.
 */
void LaunchSimulation(){
  int i, j, k;
  Message msg, msgBis;

  // Rounds loop
  for(i = 0; i < sys->nb_rounds || sys->nb_rounds < 0; i++){
    printf("Starting round %i\n", i);
    readExternalEvents();

    // Apply node function to all nodes.
    for(j = 0; j < sys->nb_nodes; j++)
      (sys->fun)(j, sys->nodes[j].receiv);

    //clear reception buffer
    for(j = 0; j < sys->nb_nodes; j++)
      sys->nodes[j].receiv = NULL;

    for(j = 0; j < sys->nb_nodes; j++){
      //msg is the older message sended by j 
      //or NULL if no messages 
      msg = GetHead(sys->nodes[j].sendBuf);
      if(NULL != msg){
        if(-1 != msg->receiv){
          //msg is a unicast
          if(msg->receiv >= sys->nb_nodes || msg->receiv < -1){
            fprintf(stderr,"Message send to inexistant receiver ignored\n");
            break;
          }

	  //check id reception is possible
          if(NULL == sys->nodes[msg->receiv].receiv){
            sys->nodes[msg->receiv].receiv = msg;
	    RemoveHead(sys->nodes[j].sendBuf);
	  }
        }else{
          //msg is a multicast (non reliable)
          for(k = 0; k < sys->nb_nodes; k++){
            if(k != j && NULL == sys->nodes[k].receiv){
	      msgBis = copyMessage(msg);
              sys->nodes[k].receiv = msgBis;
            }
          }

	  //remove message from the queue
	  RemoveHead(sys->nodes[j].sendBuf);
          deleteMessage(msg);
        }
      }
    }
  }
}

