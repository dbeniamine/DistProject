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
    void * data; /* This structure is usefull if the 
                  * user want to define a persistent 
                  * set of data for its nodes
                  */ 
}Node;

/*
 * Type of the functions that run on the nodes and implement the different
 * policies.
 * id  : unique id of the process in [0,NbProcess].
 * m   : either NULL or the message received at the beginning of the turn.
 */
typedef void(*NodesFct)(int id, Message m);

/*
 * Initializes the system.
 * nb_nodes  : number of nodes to create.
 * nb_rounds : number of rounds to run.
 * fun       : function to execute by the nodes.
 */
void initSystem(int nb_nodes, int nb_rounds, NodesFct fun);

/*
 * Delete the system.
 */
void deleteSystem(void);

/*
 * Function used by a node to send a message m which must be correcly
 * initialized.
 * m   : the message to send.
 * Returns 0 on success.
 * Returns 1 on failure (m is NULL)
 */
int Send(Message m);

/*
 * Function used by a node to get its next externel event
 * id : the node id
 * Returns the string correponding to the next event if any or NULL
 */
char *getNextExternalEvent(int id);
 
/*
 * Function used by a node to get the total number of nodes in the system.
 * Returns the number of nodes in the system.
 */
int getNbNodes(void);

/*
 * Read external events on the standard input.
 * Events are added directly to the nodes event Fifo in the system.
 */
void readExternalEvents(void);

/*
 * Function used by a node to create its own 
 * persistent data set
 * id : the node id
 * data : the new data set
 * Return the previous data set, or NULL
 */
void *setData(int id,void *data);

/*
 * Function used by a node to acces its own
 * persistent data set
 * id : the node id
 * Returns the data set or NULL
 */
void *getData(int id);

/*
 * Core simulation function.
 * The external events are read before each round. They are handled later by
 * the node function.
 */
void LaunchSimulation(void);

#endif //__SIMULATOR_H__
