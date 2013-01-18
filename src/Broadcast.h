/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * This file defines the differnet node functions implementing different    *
 * broadcast protocols.                                                     *
 * Author: Rodolphe Lepigre                                                 *
 ****************************************************************************/
#include"Simulator.h"

/*
 * Basic broadcast. The broadcast takes N turns, all sends are done by the
 * same node (one at each turn).
 * Note that like every functions defined here, they have the type NodesFct.
 */
void BasicBroadcast(int id, Message m);

/*
 * Tree broadcast. This protocol needs log(NbNodes) turn to broadcast.
 * Every node sends to its succesors.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void TreeBroadcast(int id, Message m);

/*
 * Pipeline broadcast. This protocol takes 1 turn to broadcast, the node
 * performing the broadcast sends the message to the next note, which in turn
 * transmits the message to its neighbour and so on.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void  PipelineBroadcast(int id, Message m);

/*
 * Total Order Broadcast with good latency.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void TOBLatencyBroadcast(int id, Message m);

/*
 * Total Order Broadcast with good throughput.
 * Note that like every functions defined here, they have the type NodesFct.
 */
void TOBThroughputBroadcast(int id, Message m);
