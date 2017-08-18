#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "decode.h"
#include "splay.h"
#include "lgraph.h"
#include "haversine.h"
#include "min_heap.h"

#define DIRECTED 1
#define UNDIRECTED 0
#define MAX 99999.99
void display_zerg(const void * data);
void get_health(struct node * root, int health);
void initialize_graph(graph_ptr graph, struct node * zerg, struct tree * pcaps);
void get_all_edges(graph_ptr graph, struct node * zerg, struct node * root);
void adding_nodes(graph_ptr graph, struct node * zerg, struct node * root);
double get_distance(double src_lat, double src_long, double dest_lat, double dest_long);
void find_reachable(graph_ptr graph, const int start, bool reach[]);

void shortest_path( graph_ptr graph, int start_vertex, int graph_nodes );

int main(int argc, char **argv)
{
	int option, min_health;
	option = getopt(argc, argv, "h:");
	printf("option : %c \n", option == -1? 'T':option);
	printf("option arg: %d \n", option == -1 ? 'T':atoi(optarg));
	
	switch (option)
	{
		case 'h': 
			min_health = atoi(optarg);
			break;
		default:
			min_health = 10;
			break;
	}
	
	printf("min_health: %d \n", min_health);
	
	struct tree * pcap_nodes = create_tree();
	pcap_nodes->head = decode(argc, argv, &pcap_nodes->count);
	int nodes = pcap_nodes->count;
	
	//printf("number of nodes  = %d \n", pcap_nodes->count);
	if ( pcap_nodes->head != NULL )
	{
		printf("\nsuccesful call to decode\n");
	}
	else
	{
		printf("fail to call decode\n");
	}
	
	//printf("pcap_nodes->head == NULL ? %c \n", pcap_nodes->head == NULL ? 'T':'F');
	//printf("\n\nprinting nodes\n\n");
	preOrder(pcap_nodes->head, display_zerg);
	//get_health(pcap_nodes->head, min_health);  -> HOw to handle zergs with 0 hit points.

	graph_ptr dir_graph = createGraph(nodes, DIRECTED);
	
	//printf("entering initialize \n\n");
	initialize_graph(dir_graph, pcap_nodes->head, pcap_nodes);
	//printf("exiting initialize \n");
	
	remove_tree(pcap_nodes);
	/* read files  */
		/*check for options */
	
	/* New pcap == new zerg_struct */
	/* Add destination ID */
	/* evaluate zerg packet/header information */
		/* Output health status lower than percentage with -h option / 10% by default */
	
	/* Use decode .c to build the BST */
	
	printf("\nDIRECTED GRAPH");
	displayGraph(dir_graph);
	
	
	/* GETTING THE REACHABILITY of ALL NODES */
	int i;
	bool * reach = NULL;
	//Create and initialize the bool reachability array
	reach = malloc(sizeof(bool) * nodes);
	for (i = 0; i < nodes; i++)
	{
		reach[i] = false;
	}
	
	printf("getting reachability of all nodes from a single point\n");
	find_reachable(dir_graph, 0, reach);
	
	for (i = 0; i < nodes; i++)
	{
		printf("reach[%d]=%d \n", i, reach[i]);
	} 
	
	printf("running dijkstras !!!! \n");
	//for ( int x = 0; x < pcap_nodes->count; x++)
		shortest_path(dir_graph, 0, nodes);
	
	free(reach);
	destroyGraph(dir_graph);	
	return 0;
}

void get_health(struct node * root, int min_health)
{
	//printf("here in getting all edges \n");
	if ( root != NULL)
	{
		double health_pct =  ((struct zerg*)root->key)->health.hit_points / ((struct zerg*)root->key)->health.max_points;
		if ( ( health_pct * 100) <= min_health )
			printf("health %d \n", ((struct zerg*)root->key)->health.hit_points );
		get_health(root->left, min_health);
		get_health(root->right, min_health);
	}
	return;
}

// THe Dijkstra algorithm
void shortest_path( graph_ptr graph, int start_vertex, int graph_nodes )
{
	int v = graph_nodes; // number of vertex in the graph? pass in from BST?
	
	//Warning cleared -> Thanks to Dr. Oberts!!!
	double * distance = malloc(sizeof(double) * v);
	//int end_vertex = 1;
	
	//create a min heap
	struct MinHeap * minHeap = createMinHeap(v);
	printf("here\n");
	//initialize min heap with all vertices dist value of all vertex
	for (int x = 0; x < v ; ++x )
	{
		distance[x] = MAX;
		printf("vertex->src_ID :%u \n", graph->adjListArr[x].head->src_ID);
		minHeap->array[x] = newMinHeapNode(x, graph->adjListArr[x].head->src_ID, distance[v]);
		minHeap->pos[x] = x;
	}	
	printf("end\n");
	minHeap->array[start_vertex] = newMinHeapNode(start_vertex, graph->adjListArr[start_vertex].head->src_ID, distance[start_vertex]);
	minHeap->pos[start_vertex] = start_vertex;
	distance[start_vertex] = 0;
	decreaseKey(minHeap, start_vertex, distance[start_vertex]);
	
	minHeap->size = v;
	//printf("start = %d \n", start_vertex);
	printf("\n");
	while( !isEmpty(minHeap))
	{
		//Extract the vertex with minimum distance value
		struct MinHeapNode * minHeapNode = extractMin(minHeap);
		int u = minHeapNode->v;
		
		//Traverse through all adjacent vertices of u (the extracted
		// vertex) and update their distance values.
		printf("vertex = %d \n", u);
		struct adjlist_node * pCrawl = graph->adjListArr[u].head;
		while( pCrawl != NULL)
		{
			int v = pCrawl->vertex;
			
			//If shortest distance to is not finalized yet, and distance to v 
			// through u is less than its previously calculated distance
			//printf("distance u : %f \n", distance[u]);
			//printf("less than MAX %f : %c \n", MAX,  distance[u] < MAX ? 'T':'F');
			if ( isInMinHeap(minHeap, v) && (distance[u] < MAX) && (pCrawl->distance + distance[u] < distance[v]) )
			{
				distance[v] = distance[u] + pCrawl->distance;
				//printf("v = %d \n", v);
				//UPDATE DISTANCE VALUE in min heap also
				decreaseKey(minHeap, v, distance[v]);
				/*
				if ( v == end_vertex )
				{
					//printf("here\n");
					while( extractMin(minHeap) != NULL)
						;
					break;
				}
				*/
			}
			pCrawl = pCrawl->next;
		}
	}
	
	destroyHeap(minHeap);
	printf("\n");
	printArr(distance, v, start_vertex);
	printf("\n");
}



void find_reachable(graph_ptr graph, const int current, bool reach[])
{
	reach[current] = true;
	//printf("current = %d \n", current);
	//printf("
	adjlist_node_ptr start = graph->adjListArr[current].head;
	//printf("here  %d \n", start->vertex);
	//printf("here  %u \n", start->src_ID);
	//printf("here  %f \n", start->distance);
	
	for (;start;start = start->next) 
	{
		//printf("start->vertex = %d \n", start->vertex);
		
		if( !reach[start->vertex] ) 
		{
			find_reachable(graph,start->vertex,reach);
		}
	}
}

double get_distance(double src_lat, double src_long, double dest_lat, double dest_long)
{
	printf("\n\nin get_distance\n");
	//printf("zerg_src->lat %f \n", src_lat );
	//printf("zerg_dest->lat %f \n", dest_lat );
	return haversine_dist(src_lat, src_long, dest_lat, dest_long);
}


void initialize_graph(graph_ptr graph, struct node * zerg, struct tree * pcaps)
{
	if ( zerg != NULL)
	{
		//display_zerg(zerg->key);
		//printf("zerg_id = %u \n", ((struct zerg*)zerg->key)->srcID );
		get_all_edges(graph, zerg, pcaps->head);
		initialize_graph(graph, zerg->left, pcaps);
		initialize_graph(graph, zerg->right, pcaps);
	}
}


void get_all_edges(graph_ptr graph, struct node * zerg, struct node * root)
{
	//printf("here in getting all edges \n");
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
	
	//printf("\n\nsrc\n");
	//printf("zerg_src->lat %f \n", ((struct zerg*)zerg->key)->position.latitude.value );
	//printf("zerg_src->long %f \n", ((struct zerg*)zerg->key)->position.longitude.value );
	
	double zerg_src_lat = ((struct zerg*)zerg->key)->position.latitude.value ;
	double zerg_src_long = ((struct zerg*)zerg->key)->position.longitude.value;
	int src_vertex = ((struct zerg*)zerg->key)->number;
	
	//printf("\n\ndest\n");
	//printf("zerg_dest->lat %f \n", ((struct zerg*)root->key)->position.latitude.value );
	//printf("zerg_dest->long %f \n", ((struct zerg*)root->key)->position.longitude.value );
	
	double zerg_dest_lat = ((struct zerg*)root->key)->position.latitude.value;
	double zerg_dest_long = ((struct zerg*)root->key)->position.longitude.value;
	int dest_vertex = ((struct zerg*)root->key)->number;
	
	double distance = 0.00;
	if ( root != NULL)
	{
		//get_distance returns value in km -> multiply by 1000 to convert to meters.
		distance = ( get_distance( zerg_src_lat, zerg_src_long, zerg_dest_lat, zerg_dest_long) * 1000);
		//If distance meets minimum distance ( 1.25 yds * 0.9144) converts to meters.
		if ( distance  > ( 1.25  * 0.9144)) 
		{
			printf("distance = %f \n", distance);
			//If distance does not exceed max distance of 15 meters.
			if (distance > 15)
			{
				printf("over 15 mdistance = %f \n", distance);
			
				return;
			}
			printf("distance is withing range!!!! \n");
			addEdge(graph, src_vertex, ((struct zerg*)zerg->key)->srcID, dest_vertex, ((struct zerg*)root->key)->srcID, distance);
			//printf("after addEdge\n");
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
