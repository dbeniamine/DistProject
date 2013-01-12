/****************************************************************************
 *                 Distributed Systems: Network simulator                   *
 * Main program, handles command line arguments.                            *
 * Author: Rodolphe Lepigre                                                 *
 ****************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "Broadcast.h"
#include "Simulator.h"

/*
 * Display the help message.
 * output : file to write to (usually stdout or stderr).
 * pname  : the program name (usually argv[0]).
 */
void display_help(FILE* output, char* pname){
  fprintf(output, "Usage: %s [-N n][-R n][-h][-b|t|i|p]\n", pname);
  fprintf(output, "Possible arguments:\n");
  fprintf(output, "\t-N n\tSpecify a number of nodes n, default is 4.\n");
  fprintf(output, "\t-R n\tSpecify a number of rounds n, default is 20.\n");
  fprintf(output, "\t-h\tDisplay this help message.\n");
  fprintf(output, "Selection of broadcast mode:\n");
  fprintf(output, "\t-i\tIP broadcast (default).\n");
  fprintf(output, "\t-b\tBasic broadcast.\n");
  fprintf(output, "\t-t\tTree broadcast.\n");
  fprintf(output, "\t-L\tTotal order broadcast with good latency.\n");
  fprintf(output, "\t-T\tTotal order broadcast with good throughput.\n");
}

/*
 * Main function. Parses command line arguments and launchs the simulator.
 */
int main (int argc, char **argv){
  int opt;
  System sys;

  // Default values
  NodesFct f = IPBroadcast;
  int nb_nodes = 4;
  int nb_rounds = 20;

  // Parsing arguments
  while(-1 != (opt = getopt(argc, argv, "N:R:hibtpLT"))){
    switch(opt){
      case 'N':
        nb_nodes = atoi(optarg);
        break;
      case 'R':
        nb_rounds = atoi(optarg);
        break;
      case 'h':
        display_help(stdout, argv[0]);
        return 0;
      case 'i':
        f = IPBroadcast;
        break;
      case 'b':
        f = BasicBroadcast;
        break;
      case 't':
        f = TreeBroadcast;
        break;
      case 'p':
        f = PipelineBroadcast;
        break;
      case 'L':
        f = TOBLatencyBroadcast;
        break;
      case 'T':
        f = TOBThroughputBroadcast;
        break;
      default: /* WTF ? */
        fprintf(stderr, "Argument error...\n");
        display_help(stderr, argv[0]);
        exit(EXIT_FAILURE);
      }
  }

  // Do the simulation
  sys = initSystem(nb_nodes, nb_rounds, f);
  LaunchSimulation(sys);
  deleteSystem(sys);
  return 0;
}
