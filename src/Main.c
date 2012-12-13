#include"Simulator.h"

#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<unistd.h>

int NbNodes;

//Tree broadcast: log(NbNodes) turn to broadcast
//every node send to its succesors
void TreeBroadcast(int Id, Message_t m, Fifo events){
    Message_t event, msgOut;
    int nTurn;
    //Events Rules
    while((event=RemoveHead(events))!=NULL){
        printf("event received %d %s\n",Id, event->msg); 
        //Read the first event
        if(!strcmp(event->msg,"broadcast")){
            //start a tree broadcast:
            //Iniatialize the message
	    for(nTurn = 0; nTurn < log2(NbNodes); nTurn++){
                //at the first step of the tree broadcast, we send a message
                //to our successor
                msgOut = initMessage("Hello\0", Id, (int)(pow(2,nTurn)+Id)%NbNodes);
                Send(msgOut);
            }
        }
        free(event->msg);
        free(event);
    }
    //Message Rules
    if(m!=NULL){
        printf("%s received by %d from %d\n", m->msg, Id, m->sender);
        //at a step n, a message is sent at a distance 2^n
        //the distance is not exactly the difference between the sender and the
        //receiver id because of the mod N
        if(m->sender < m->receiv)
            nTurn=log2(m->receiv-m->sender);
        else
            nTurn=log2(NbNodes-m->sender+m->receiv);
        //this turn is done, let's do the others
        //now we can send all the others messages
        for(nTurn++; nTurn<log2(NbNodes); nTurn++){
            msgOut = initMessage(m->msg, Id, ((int)(pow(2,nTurn)+Id))%NbNodes);
            Send(msgOut);
        }
        free(m->msg);
        free(m);
    }
}

//Ip broadcast: one node send to everyone on one round
void IPBroadcast(int Id, Message_t m, Fifo events){
    Message_t event, msgOut;
    //Events Rules
    while((event=RemoveHead(events))!=NULL){
        printf("event received %d %s\n",Id, event->msg); 
        //Read the first event
        if(!strcmp(event->msg,"broadcast")){
            //start a basic broadcast:
            //send hello to every nodes
            //Iniatialize the message
	    msgOut = initMessage("Hello\0", Id, -1);
            Send(msgOut);
        }
        free(event->msg);
        free(event);
    }
    //Message Rules
    if(m!=NULL){
        printf("%s received by %d from %d\n", m->msg, Id, m->sender);
        free(m->msg);
        free(m);
    }
}

//Basic broadcast: N turn to broadcast, all send are done by
//the same node
void BasicBroadcast(int Id, Message_t m, Fifo events){
    Message_t event, msgOut;
    int i;
    //Events Rules
    while((event=RemoveHead(events))!=NULL){
        printf("event received %d %s\n",Id, event->msg); 
        //Read the first event
        if(!strcmp(event->msg,"broadcast")){
            //start a basic broadcast:
            //send hello to every nodes
            for(i=0;i<NbNodes;i++){
                if(i!=Id){
		    msgOut = initMessage("Hello\0", Id, i);
                    Send(msgOut);
                }
            }
        }
        free(event->msg);
        free(event);
    }
    //Message Rules
    if(m!=NULL){
        printf("%s received by %d from %d\n", m->msg, Id, m->sender);
        free(m->msg);
        free(m);
    }
}

// Pipeline broadcast: 1 turn to broadcast, the broadcasting node sends his
// message to the next note, which in turn transmits the message to the next
// node and so on.
void PipelineBroadcast(int Id, Message_t m, Fifo events){
    Message_t event, msg, fwd;
    int neighbor, broadcaster;
    char* content;

    content = malloc(128 * sizeof(char));
    if(NULL == content){
        fprintf(stderr, "Failed malloc in PipelineBroadcast...\n");
	exit(EXIT_FAILURE);
    }

    // Event Rules
    while(NULL != (event = RemoveHead(events))){
        printf("event received %i %s\n",Id, event->msg); 

        // Read the first event
        if(0 == strcmp(event->msg, "broadcast")){
            neighbor = (Id + 1) % NbNodes;
            sprintf(content, "from %i : Hello", Id);
            msg = initMessage(content, Id, neighbor);
            Send(msg);
	}

	free(event->msg);
	free(event);
    }

    // Message Rules
    if(NULL != m)
        if(1 > sscanf(m->msg, "from %i : %s", &broadcaster, content)){
            // Print content and data
            printf("%s received by %i from %i (broadcasted by %i)\n",
	           content, Id, m->sender, broadcaster);

	    // Forward the message if need be
	    neighbor = (Id + 1) % NbNodes;
	    if(neighbor != broadcaster){
                fwd = initMessage(m->msg, Id, neighbor);
		Send(fwd);
	    }

	    // Free the local message
            free(m->msg);
            free(m);
	}

    free(content);
}

void display_help(FILE* output, char* pname){
    fprintf(output, "Usage: %s [-N n][-R n][-h][-b|t|i|p]\n", pname);
    fprintf(output, "Possible arguments:\n");
    fprintf(output, "\t-N n\tSpecify a number of nodes n, default is 4.\n");
    fprintf(output, "\t-R n\tSpecify a number of rounds n, default is 20.\n");
    fprintf(output, "\t-h\tDisplay this help message.\n");
    fprintf(output, "Selection of broadcast mode:\n");
    fprintf(output, "\t-b\tBasic broadcast.\n");
    fprintf(output, "\t-t\tTree broadcast.\n");
    fprintf(output, "\t-i\tIP broadcast (default).\n");
    fprintf(output, "\t-p\tPipeline broadcast.\n");
}

int main (int argc, char **argv){
    NodesFct_t f;
    int opt, nb_nodes, nb_rounds;

    // Default values
    f = IPBroadcast;
    nb_nodes = 4;
    nb_rounds = 20;

    // Parsing arguments
    while(-1 != (opt = getopt(argc, argv, "N:R:hbtip"))){
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
	case 'b':
	    f = BasicBroadcast;
	    break;
	case 't':
	    f = TreeBroadcast;
	    break;
	case 'i':
	    f = IPBroadcast;
	    break;
	case 'p':
	    f = PipelineBroadcast;
	    break;
	default: /* WTF ? */
	    fprintf(stderr, "Argument error...\n");
	    display_help(stderr, argv[0]);
	    exit(EXIT_FAILURE);
	}
    }

    // Start a simulation
    LaunchSimulation(nb_rounds, nb_nodes, f);
    return 0;
}

