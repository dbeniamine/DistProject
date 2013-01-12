/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * This file contains the core functions of the simulator.                  *
 * Author: David Beniamine                                                  *
 ****************************************************************************/
#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include "Fifo.h"
#include "Message.h"

/*
 * Structure represeting a node.
 */
typedef struct _Node{
    Fifo receivBuf;
    Fifo sendBuf;
    Fifo eventsBuf;
}*Node;

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

#endif //__SIMULATOR_H__
