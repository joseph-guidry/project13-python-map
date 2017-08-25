#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "decode.h"
#include "splay.h"
#include "lgraph.h"
#include "haversine.h"
#include "zerg_connect.h"

#define DIRECTED 1
#define UNDIRECTED 0

void display_zerg(const void * data);
void display_removed(const void * data);
void get_health(struct node * root, int health);

void initialize_graph(graph_ptr graph, struct node * zerg, struct tree * pcaps);
void get_all_edges(graph_ptr graph, struct node * zerg, struct node * root);
double get_distance(double src_lat, double src_long, double src_alt, double dest_lat, double dest_long, double dest_alt);
void adding_nodes(graph_ptr graph, struct node * zerg, struct node * root);
struct node * removeNode(graph_ptr graph, int node, struct node * root, int * count );

void find_reachable(graph_ptr graph, const int start, bool reach[]);
void zerg_connected(graph_ptr graph, int start, int end, int num_nodes);
int check_first_vertex(graph_ptr graph, int nodes);
int check_last_vertex(graph_ptr graph, int nodes);

int main(int argc, char **argv)
{
	int option, min_health;
	option = getopt(argc, argv, "h:");
	switch (option)
	{
		case 'h': 
			min_health = atoi(optarg);
			break;
		default:
			min_health = 10;
			break;
	}
	
	struct tree * pcap_nodes = create_tree();
	pcap_nodes->head = decode(argc, argv, &pcap_nodes->count);
	int nodes = pcap_nodes->count;
	
	printf("Display zergs health percentage from data received from input below %d %%\n", min_health);
	get_health(pcap_nodes->head, min_health);  
	
	//Create and initialize graph with data from tree data structure.
	graph_ptr dir_graph = createGraph(nodes, DIRECTED);
	initialize_graph(dir_graph, pcap_nodes->head, pcap_nodes);
	
	// Determine if there is enough nodes to build graph
	if ( nodes <= 2 )
	{
		printf("not enough nodes to build a graph from pcaps\n");
		preOrder(pcap_nodes->head, display_zerg);
		remove_tree(pcap_nodes);
		return 2;
	}

	int last_vertex;
	int first_vertex;
	// Get the first index position.
	first_vertex = check_first_vertex(dir_graph, nodes);
	//Find the last vertex in the graph
	last_vertex = check_last_vertex(dir_graph, nodes);
	//printf("first = %d\nlast = %d\n", first_vertex, last_vertex);
	
	/* GETTING THE REACHABILITY of ALL NODES */
	int i;
	bool * reach = NULL;
	//Create and initialize the bool reachability array
	reach = malloc(sizeof(bool) * nodes);
	for (i = 0; i < nodes; i++)
	{
		reach[i] = false;
	}
	
	//Create a tree to hold the removed zergs src_id;
	struct tree * dead_zerg = create_tree();
	dead_zerg->head = NULL;
	
	// Remove zergs that are not reachable from a single source.
	find_reachable(dir_graph, first_vertex, reach);
	for (i = 0; i < nodes; i++)
	{
		if ( reach[i] == 0)
		{
			if ( !(dir_graph->adjListArr[i].head == NULL) )
			{
				//If node is unreachable, remove it from the graph -> add to BST.
				dead_zerg->head = removeNode(dir_graph, i, dead_zerg->head, &dead_zerg->count);
			}
		}
	}
	// Release the malloc'd space for reach array
	free(reach);
	
	// Remove zergs with only a single edge
	for ( i = 0; i < nodes; i++)
	{
		if(  dir_graph->adjListArr[i].num_members < 2)
		{
			if ( !(dir_graph->adjListArr[i].head == NULL) )
			{
				dead_zerg->head = removeNode(dir_graph, i, dead_zerg->head, &dead_zerg->count);
			}
		}
	}
	// Check if the number of zergs removed is over 50% of the total number of zergs.
	if (  ( (dead_zerg->count / (double)nodes ) * 100) > 50.0)
	{
		printf("Removed more than 50%% of zergs : Not Zerg Connected \n");
		preOrder(dead_zerg->head, display_removed);
		return 1;
	}
	
	int updated_node_count = 0;
	// GEt an updated number of valid nodes in the graph prior to path-finding.
	for ( int x = 0; x < nodes; x++)
	{
		if ( (dir_graph->adjListArr[x].head != NULL) )
		{
			updated_node_count++;
		}
	}
	
	// Check the first vertex in the graph
	first_vertex = check_first_vertex(dir_graph, nodes);
	// Check the last vertex in the graph
	last_vertex = check_last_vertex(dir_graph, nodes);
	
	if ( updated_node_count == 0 )
	{
		printf("No zergs in the graph \n");
	}
	else
	{
		//Attempt to determine zerg connected by getting 2 disjoint paths from index 0 to last useable vertex;
		zerg_connected(dir_graph, first_vertex, last_vertex, updated_node_count);
	}

	//Output zergs that were removed from the graph
	preOrder(dead_zerg->head, display_removed);
	
	//Clean up data structures and release memory
	destroyGraph(dir_graph);
	remove_tree(pcap_nodes);
	remove_tree(dead_zerg);
	if ( dead_zerg == NULL)
		free(dead_zerg);
	
	return 0;
}

int check_first_vertex(graph_ptr graph, int nodes)
{
	int first_vertex;
	for ( int x = 0; x < nodes; x++)
	{
		if ( (graph->adjListArr[x].head != NULL) )
		{
			first_vertex = x;
			break;
		}
	}
	return first_vertex;
}

int check_last_vertex(graph_ptr graph, int nodes)
{
	int last_vertex;
	
	for ( int x = 0; x < nodes; x++)
		{
			if ( !(graph->adjListArr[x].head == NULL) )
			{
				last_vertex = x;
			}
		}
		
	return last_vertex;
}
void get_health(struct node * root, int min_health)
{
	if ( root != NULL)
	{
		double health_pct = ((struct zerg*)root->key)->health.hit_points / (double)((struct zerg*)root->key)->health.max_points;
		if ( ( health_pct * 100) <= min_health )
			//Output the health as a percentage.
			printf("zerg: %3d has health of %0.2f%% \n", ((struct zerg*)root->key)-> srcID, health_pct * 100);
		get_health(root->left, min_health);
		get_health(root->right, min_health);
	}
	return;
}

//Ensure all nodes are reachable from a given start point.
void find_reachable(graph_ptr graph, const int current, bool reach[])
{
	reach[current] = true;
	adjlist_node_ptr start = graph->adjListArr[current].head;
	
	for (;start;start = start->next) 
	{
		if( !reach[start->vertex] ) 
		{
			find_reachable(graph,start->vertex,reach);
		}
	}
}

//Function to get haversine equation results.
double get_distance(double src_lat, double src_long, double src_alt, double dest_lat, double dest_long, double dest_alt)
{
	return haversine_dist(src_lat, src_long, src_alt, dest_lat, dest_long, dest_alt);
}

//Gather data from BST into the graph Adj. List Array
void initialize_graph(graph_ptr graph, struct node * zerg, struct tree * pcaps)
{
	if ( zerg != NULL)
	{
		get_all_edges(graph, zerg, pcaps->head);
		initialize_graph(graph, zerg->left, pcaps);
		initialize_graph(graph, zerg->right, pcaps);
	}
}


void get_all_edges(graph_ptr graph, struct node * zerg, struct node * root)
{
	if ( root != NULL)
	{
		adding_nodes(graph, zerg, root);
		get_all_edges(graph, zerg, root->left);
		get_all_edges(graph, zerg, root->right);
	}
	return;
}

//From each point, attempt to add edge that meet minimum distance requirement.
void adding_nodes(graph_ptr graph, struct node * zerg, struct node * root)
{
	
	double src_lat = ((struct zerg*)zerg->key)->position.latitude.value ;
	double src_long = ((struct zerg*)zerg->key)->position.longitude.value;
	double src_alt = ((struct zerg*)zerg->key)->position.altitude.value;
	int src_vertex = ((struct zerg*)zerg->key)->number;
	
	// Do not process data with gps data out of range
	if ( abs(src_lat) > 90 || abs(src_long) > 180)
	{
		return;
	}
	
	double dest_lat = ((struct zerg*)root->key)->position.latitude.value;
	double dest_long = ((struct zerg*)root->key)->position.longitude.value;
	double dest_alt = ((struct zerg*)root->key)->position.altitude.value;
	int dest_vertex = ((struct zerg*)root->key)->number;
	
	// Do not process data with gps data out of range
	if ( abs(dest_lat) > 90 || abs(dest_long) > 180)
	{
		return;
	}
	
	double distance = 0.00;
	if ( root != NULL)
	{
		//get_distance returns value in km -> multiply by 1000 to convert to meters.
		distance = ( get_distance(src_lat, src_long, src_alt, dest_lat, dest_long, dest_alt) * 1000);
		//If distance meets minimum distance ( 1.25 yds * 0.9144) converts to meters.
		if ( distance  > ( 1.25  * 0.9144)) 
		{
			//If distance does not exceed max distance of 15 meters.
			if (distance > 15)
			{
				return;
			}
			addEdge(graph, src_vertex,((struct zerg*)zerg->key)->srcID, dest_vertex,((struct zerg*)root->key)->srcID, distance);
		}
	}
	return;
}

struct node * removeNode(graph_ptr graph, int node, struct node * root, int * count )
{
	//Create a node for the node to be inserted
	root = insert(root, graph->adjListArr[node].src_ID, sizeof(int), get_root_srcID);
	(*count)++;
	//Assign the data to the node
	*((uint16_t*)(root->key)) = graph->adjListArr[node].src_ID;
	while ( graph->adjListArr[node].head != NULL)
	{
		int edge_node = graph->adjListArr[node].head->vertex;
		removeEdge(graph, node, edge_node);
		removeEdge(graph, edge_node, node);
	}
	return root;
}

void display_zerg(const void * data)
{
	printf("display node\n");
	printf("zerg_id: %u \n", ((struct zerg*)data)->srcID );
	printf("zerg_lat: %06f \n",  ((struct zerg*)data)->position.latitude.value );
	printf("zerg_long: %06f \n",  ((struct zerg*)data)->position.longitude.value );
	printf("zerg_alt: %06f \n",  ((struct zerg*)data)->position.altitude.value  * 1.8288);
	printf("\n");
}
void display_removed(const void * data)
{
	printf("removed node: ");
	printf("zerg: [%u] \n", *((uint16_t*)(data)) );
}
