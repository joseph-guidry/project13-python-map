#include <stdio.h>
#include <stdlib.h>

//#include "zerg.h"
#include "decode.h"
#include "splay.h"
#include "lgraph.h"

#define DIRECTED 1
#define UNDIRECTED 0

void display_zerg(const void * data);

int main(int argc, char **argv)
{
	struct tree * pcap_nodes = create_tree();
	pcap_nodes->head = decode(argc, argv, pcap_nodes->count);
	if ( pcap_nodes->head != NULL )
	{
		printf("\nsuccesful call to decode\n");
	}
	else
	{
		printf("fail to call decode\n");
	}
	
	printf("pcap_nodes->head == NULL ? %c \n", pcap_nodes->head == NULL ? 'T':'F');
	printf("\n\nprinting nodes\n\n");
	preOrder(pcap_nodes->head, display_zerg);
	
	graph_ptr undir_graph = createGraph(5, UNDIRECTED);
	
	/* read files  */
		/*check for options */
	
	/* New pcap == new zerg_struct */
	/* Add destination ID */
	/* evaluate zerg packet/header information */
		/* Output health status lower than percentage with -h option / 10% by default */
	
	/* Use decode .c to build the BST */
	/*
	//Adding to undirected graph 
	addEdge(undir_graph, 0, 1);
	addEdge(undir_graph, 0, 4);
	addEdge(undir_graph, 1, 2);
	addEdge(undir_graph, 1, 3);
	addEdge(undir_graph, 1, 4);
	addEdge(undir_graph, 2, 3);
	addEdge(undir_graph, 3, 4);
	
	// Adding to directed graph 
	addEdge(dir_graph, 0, 1);
	addEdge(dir_graph, 0, 4);
	addEdge(dir_graph, 1, 2);
	addEdge(dir_graph, 1, 3);
	addEdge(dir_graph, 1, 4);
	addEdge(dir_graph, 2, 3);
	addEdge(dir_graph, 3, 4);
	
	printf("\nUNDIRECTED GRAPH");
	displayGraph(undir_graph);
	destroyGraph(undir_graph);
	
	printf("\nDIRECTED GRAPH");
	displayGraph(dir_graph);
	destroyGraph(dir_graph);
	*/

	
	/*decode.c returns BST */
	
	/* use bst to build graph */
	return 0;
}



void display_zerg(const void * data)
{
	printf("display node\n");
	printf("zerg_id: %u \n", ((struct zerg*)data)->srcID );
	printf("zerg_id: %06f \n",  ((struct zerg*)data)->position.latitude.value );
	printf("zerg_id: %06f \n",  ((struct zerg*)data)->position.longitude.value );
	printf("zerg_id: %06f \n",  ((struct zerg*)data)->position.altitude.value );
	printf("\n");
}
