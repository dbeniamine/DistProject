#ifndef __SIMULATOR_H
#define __SIMULATOR_H__
#include"Fifo.h"

//Message structur, each field have to be initialized before any send
typedef struct _Message{
    int sender; //if -1, msg is an external event
    int receiv; //if -1 : ip multicast
    char *msg;
}*Message_t;

// Message initialization function.
//   str : content of the message
//   snd : sender of the message
//   rcv : receiver of the message
// Failures are handled internaly.
// Returns a Message_t object.
Message_t initMessage(const char* str, int snd, int rcv);

// Delete a message.
//   msg : the message
// Failures are handled internaly.
void deleteMessage(Message_t msg);

//Algorithm run by each process
//  id    : the unique id of the process in [0,NbProcess]
//  m     : either NULL or the message received at the beginning of the turn
//  queue :  the list of external event which happens to the node at the
//           beggining of the round

typedef void(*NodesFct_t)(int id , Message_t m,Fifo queue);

//A node can use this method to send a message m which must be correcly
//initialized with the method initMessage, return 0 on success
int Send(Message_t m);


//Start a simulation 
//  NbProcess   : the number of nodes
//  NbRounds    : the number of rounds (if <=0 forever)
//  f           : the function run by each node
//The external events are read by the simulator on the standard input
void LaunchSimulation(int NbRounds, int NbProcess, NodesFct_t f);


#endif //__SIMULATOR_H

