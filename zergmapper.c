#include <stdio.h>
#include <stdlib.h>

#include "decode.h"
#include "splay.h"
#include "lgraph.h"
#include "haversine.h"

#define DIRECTED 1
#define UNDIRECTED 0

void display_zerg(const void * data);
void initialize_graph(graph_ptr graph, struct node * zerg, struct tree * pcaps);
void get_all_edges(graph_ptr graph, struct node * zerg, struct node * root);
void adding_nodes(graph_ptr graph, struct node * zerg, struct node * root);
double get_distance(double src_lat, double src_long, double dest_lat, double dest_long);

int main(int argc, char **argv)
{
	struct tree * pcap_nodes = create_tree();
	pcap_nodes->head = decode(argc, argv, &pcap_nodes->count);
	printf("number of nodes  = %d \n", pcap_nodes->count);
	if ( pcap_nodes->head != NULL )
	{
		printf("\nsuccesful call to decode\n");
	}
	else
	{
		printf("fail to call decode\n");
	}
	
	printf("pcap_nodes->head == NULL ? %c \n", pcap_nodes->head == NULL ? 'T':'F');
	//printf("\n\nprinting nodes\n\n");
	//preOrder(pcap_nodes->head, display_zerg);
	
	graph_ptr dir_graph = createGraph(pcap_nodes->count, DIRECTED);
	
	printf("entering initialize \n\n");
	initialize_graph(dir_graph, pcap_nodes->head, pcap_nodes);
	printf("exiting initialize \n");
	
	printf("\nUNDIRECTED GRAPH");
	displayGraph(dir_graph);
	printf("\n");
	
	/* read files  */
		/*check for options */
	
	/* New pcap == new zerg_struct */
	/* Add destination ID */
	/* evaluate zerg packet/header information */
		/* Output health status lower than percentage with -h option / 10% by default */
	
	/* Use decode .c to build the BST */
	
	printf("\nDIRECTED GRAPH");
	displayGraph(dir_graph);
	destroyGraph(dir_graph);
	

	
	return 0;
}


double get_distance(double src_lat, double src_long, double dest_lat, double dest_long)
{
	printf("\n\nin get_distance\n");
	printf("zerg_src->lat %f \n", src_lat );
	printf("zerg_dest->lat %f \n", dest_lat );
	return haversine_dist(src_lat, src_long, dest_lat, dest_long);
	
}


void initialize_graph(graph_ptr graph, struct node * zerg, struct tree * pcaps)
{
	if ( zerg != NULL)
	{
		//display_zerg(zerg->key);
		printf("zerg_id = %u \n", ((struct zerg*)zerg->key)->srcID );
		
		get_all_edges(graph, zerg, pcaps->head);
		
		initialize_graph(graph, zerg->left, pcaps);
		initialize_graph(graph, zerg->right, pcaps);
	}
}


void get_all_edges(graph_ptr graph, struct node * zerg, struct node * root)
{
	printf("here in getting all edges \n");
	if ( root != NULL)
	{
	adding_nodes(graph, zerg, root);
	
	get_all_edges(graph, zerg, root->left);
	get_all_edges(graph, zerg, root->right);
	
	}
	return;
}


void adding_nodes(graph_ptr graph, struct node * zerg, struct node * root)
{
	
	printf("\n\nsrc\n");
	printf("zerg_src->lat %f \n", ((struct zerg*)zerg->key)->position.latitude.value );
	printf("zerg_src->long %f \n", ((struct zerg*)zerg->key)->position.longitude.value );
	
	double zerg_src_lat = ((struct zerg*)zerg->key)->position.latitude.value ;
	double zerg_src_long = ((struct zerg*)zerg->key)->position.longitude.value;
	int src_vertex = ((struct zerg*)zerg->key)->number;
	
	printf("\n\ndest\n");
	printf("zerg_dest->lat %f \n", ((struct zerg*)root->key)->position.latitude.value );
	printf("zerg_dest->long %f \n", ((struct zerg*)root->key)->position.longitude.value );
	
	double zerg_dest_lat = ((struct zerg*)root->key)->position.latitude.value;
	double zerg_dest_long = ((struct zerg*)root->key)->position.longitude.value;
	int dest_vertex = ((struct zerg*)root->key)->number;
	
	double distance = 0.00;
	if ( root != NULL)
	{
		distance = ( get_distance( zerg_src_lat, zerg_src_long, zerg_dest_lat, zerg_dest_long) * 1000);
		if ( distance  > ( 1.25  * 0.9144)) 
		//If distance meets minimum distance;
		{
			printf("distance = %f \n", distance);
			
			if (distance > 15)
			{
				printf("over 15 mdistance = %f \n", distance);
			
				return;
			}
			printf("distance is withing range!!!! \n");
			addEdge(graph, src_vertex, ((struct zerg*)zerg->key)->srcID, dest_vertex, ((struct zerg*)root->key)->srcID, distance);
			printf("after addEdge\n");
		}
		
	}
	
	return;
}

void display_zerg(const void * data)
{
	printf("display node\n");
	printf("zerg_id: %u \n", ((struct zerg*)data)->srcID );
	printf("zerg_lat: %06f \n",  ((struct zerg*)data)->position.latitude.value );
	printf("zerg_long: %06f \n",  ((struct zerg*)data)->position.longitude.value );
	printf("zerg_alt: %06f \n",  ((struct zerg*)data)->position.altitude.value );
	printf("\n");
}
