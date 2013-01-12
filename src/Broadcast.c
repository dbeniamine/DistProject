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

/*
 * Ip broadcast. A node can send to every other node in one round.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void IPBroadcast(int id, Message m, System sys){
  char* event;
  Message msgOut;

  // Events Rules
  while((event = RemoveHead(sys->nodes[id].eventsBuf)) != NULL){
    printf("Event received %d %s\n", id, event); 
    //Read the first event
    if(!strcmp(event, "broadcast")){
      //start a basic broadcast:
      //send hello to every nodes
      //Iniatialize the message
      msgOut = initMessage("Hello\0", id, -1);
      Send(msgOut, sys);
    }
    free(event);
  }

  //Message Rules
  if(NULL != m){
    printf("%s received by %d from %d\n", m->msg, id, m->sender);
    free(m->msg);
    free(m);
  }
}

/*
 * Basic broadcast. The broadcast takes N turns, all sends are done by the
 * same node (one at each turn).
 * Note that like every functions defined here, they have the type NodesFct.
 */
void BasicBroadcast(int id, Message m, System sys){
  char* event;
  Message msgOut;
  int i;

  //Events Rules
  while((event = RemoveHead(sys->nodes[id].eventsBuf)) != NULL){
    printf("event received %d %s\n", id, event); 
    //Read the first event
    if(!strcmp(event, "broadcast")){
      //start a basic broadcast:
      //send hello to every nodes
      for(i = 0; i < sys->nb_nodes; i++){
        if(i != id){
          msgOut = initMessage("Hello\0", id, i);
          Send(msgOut, sys);
        }
      }
    }
    free(event);
  }

  //Message Rules
  if(NULL != m){
    printf("%s received by %d from %d\n", m->msg, id, m->sender);
    free(m->msg);
    free(m);
  }
}

/*
 * Tree broadcast. This protocol needs log(NbNodes) turn to broadcast.
 * Every node sends to its succesors.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void TreeBroadcast(int id, Message m, System sys){
  char* event;
  Message msgOut;
  int nTurn;

  //Events Rules
  while((event = RemoveHead(sys->nodes[id].eventsBuf)) != NULL){
    printf("Event received %d %s\n", id, event); 
    //Read the first event
    if(!strcmp(event, "broadcast")){
      //start a tree broadcast:
      //Iniatialize the message
      for(nTurn = 0; nTurn < log2(sys->nb_nodes); nTurn++){
        //at the first step of the tree broadcast, we send a message
        //to our successor
        msgOut = initMessage("Hello\0", id, (int)(pow(2,nTurn)+id)%sys->nb_nodes);
        Send(msgOut, sys);
      }
    }
    free(event);
  }

  //Message Rules
  if(NULL != m){
    printf("%s received by %d from %d\n", m->msg, id, m->sender);
    //at a step n, a message is sent at a distance 2^n
    //the distance is not exactly the difference between the sender and the
    //receiver id because of the mod N
    if(m->sender < m->receiv)
      nTurn=log2(m->receiv-m->sender);
    else
      nTurn=log2(sys->nb_nodes-m->sender+m->receiv);
    //this turn is done, let's do the others
    //now we can send all the others messages
    for(nTurn++; nTurn<log2(sys->nb_nodes); nTurn++){
      msgOut = initMessage(m->msg, id, ((int)(pow(2,nTurn)+id))%sys->nb_nodes);
      Send(msgOut, sys);
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
void PipelineBroadcast(int id, Message m, System sys){
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
  while(NULL != (event = RemoveHead(sys->nodes[id].eventsBuf))){
    printf("event received %i %s\n",id, event); 
    // Read the first event
    if(0 == strcmp(event, "broadcast")){
      neighbor = (id + 1) % sys->nb_nodes;
      sprintf(content, "from %i : Hello", id);
      msg = initMessage(content, id, neighbor);
      Send(msg, sys);
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
      neighbor = (id + 1) % sys->nb_nodes;
      if(neighbor != broadcaster){
        fwd = initMessage(m->msg, id, neighbor);
        Send(fwd, sys);
      }
 
      // Free the local message
      free(m->msg);
      free(m);
     }
  }

  free(content);
}
