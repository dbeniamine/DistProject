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
 * Structure representing a node.
 */
typedef struct _Node{
    Fifo receivBuf; // Reception buffer (contains Message).
    Fifo sendBuf;   // Send buffer (contains Message).
    Fifo eventsBuf; // Event buffer (contains char*).
}Node;

/*
 * Structure representing the system.
 * The last field holds a function to be execuded by the nodes.
 */
typedef struct _System{
  Node* nodes;      // The nodes (process).
  int nb_nodes;     // Number of nodes.
  int nb_rounds;    // Number of rounds to simulate.
  void(*fun)(int id, Message m, struct _System* sys); // Function
}*System;

/*
 * Type of the functions that run on the nodes and implement the different
 * policies.
 * id  : unique id of the process in [0,NbProcess].
 * m   : either NULL or the message received at the beginning of the turn.
 * sys : the system (contains all data like external events, number of
 *       nodes...).
 */
typedef void(*NodesFct)(int id, Message m, System sys);

/*
 * Initializes a system.
 * nb_nodes  : number of nodes to create.
 * nb_rounds : number of rounds to run.
 * fun       : function to execute by the nodes.
 * Returns a pointer to the System.
 */
System initSystem(int nb_nodes, int nb_rounds, NodesFct fun);

/*
 * Delete a system.
 * sys : the system to delete.
 */
void deleteSystem(System sys);

/*
 * Function used by a node to send a message m which must be correcly
 * initialized.
 * m   : the message to send.
 * sys : the system.
 * Returns 0 on success.
 * Returns 1 on failure (m is NULL)
 */
int Send(Message m, System sys);

/*
 * Read external events on the standard input.
 * Events are added directly to the nodes event Fifo in the system.
 * sys : the system.
 */
void readExternalEvents(System sys);

/*
 * Core simulation function.
 * The external events are read before each round. They are handled later by
 * the node function.
 * sys : the system.
 */
void LaunchSimulation(System sys);

#endif //__SIMULATOR_H__
