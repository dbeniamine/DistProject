/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * This file contains the core functions of the simulator.                  *
 * Author: David Beniamine                                                  *
 ****************************************************************************/
#ifndef __SIMULATOR_H
#define __SIMULATOR_H__

#include"Fifo.h"

/*
 * Structure implementing messages.
 */
typedef struct _Message{
  int sender; // Identifier of the sender (external event if -1)
  int receiv; // Destination address (-1 if ip multicast)
  char *msg;  // Content of the message
}*Message;

/*
 * Message initialization function.
 * str : content of the message.
 * snd : sender of the message.
 * rcv : receiver of the message.
 * Failures are handled internally.
 * Returns a the new created message.
 */
Message initMessage(const char* str, int snd, int rcv);

/*
 * Delete a message.
 * msg : the message to delete.
 * Failures are handled internally.
 */
void deleteMessage(Message msg);

/*
 * Copy a message.
 * msg : the message.
 * Failures are handled internaly.
 * Returns the copied message.
 */
Message copyMessage(Message msg);

/*
 * Type of a function executed by the nodes in the simulator.
 * Implements the policies (different broadcast protocols).
 * id    : unique id of the process in [0,NbProcess].
 * m     : either NULL or the message received at the beginning of the turn.
 * queue : list of external event happening to the node at the beggining of
 *         the round.
 */
typedef void(*NodesFct_t)(int id, Message m, Fifo queue);

/*
 * Function used by a node to send a message m which must be correcly
 * initialized.
 * m : the message to send.
 * Return 0 on success.
 * Returns 1 on failure (m is NULL)
 */
int Send(Message m);

/*
 * Core simulation function.
 * The external events are read by the simulator on the standard input.
 * NbRounds  : number of rounds to run (if <=0 run forever).
 * NbProcess : number of nodes.
 * f         : function to run by each node.
 */
void LaunchSimulation(int NbRounds, int NbProcess, NodesFct_t f);

#endif //__SIMULATOR_H
