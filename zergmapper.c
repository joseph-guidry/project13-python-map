#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "decode.h"
#include "splay.h"
#include "lgraph.h"
#include "haversine.h"
//#include "min_heap.h"
#include "zerg_connect.h"

#define DIRECTED 1
#define UNDIRECTED 0
#define MAX 99999.99

void display_zerg(const void * data);
void display_removed(const void * data);
void get_health(struct node * root, int health);
void initialize_graph(graph_ptr graph, struct node * zerg, struct tree * pcaps);
void get_all_edges(graph_ptr graph, struct node * zerg, struct node * root);
void adding_nodes(graph_ptr graph, struct node * zerg, struct node * root);
double get_distance(double src_lat, double src_long, double src_alt, double dest_lat, double dest_long, double dest_alt);
void find_reachable(graph_ptr graph, const int start, bool reach[]);

void shortest_path( graph_ptr graph, int start_vertex, int graph_nodes );
void shortest_path_v2(graph_ptr graph, int start_node, int graph_nodes);
void zerg_connected(graph_ptr graph, int start, int end, int num_nodes);
struct node * removeNode(graph_ptr graph, int node, struct node * root, int * count );

void get_health(struct node * root, int min_health)
{
	//printf("here in get_health \n");
	if ( root != NULL)
	{
		double health_pct = (double)((struct zerg*)root->key)->health.hit_points / (double)((struct zerg*)root->key)->health.max_points;
		//printf("health_pct = %f \n", health_pct);
		if ( ( health_pct * 100) <= min_health )
			printf("zerg: %d has health %d \n", ((struct zerg*)root->key)-> srcID, ((struct zerg*)root->key)->health.hit_points );
		get_health(root->left, min_health);
		get_health(root->right, min_health);
	}
	return;
}

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
	
	//printf("\n\nprinting nodes\n\n");
	//preOrder(pcap_nodes->head, display_zerg);
	get_health(pcap_nodes->head, min_health);  //-> HOw to handle zergs with 0 hit points.
	graph_ptr dir_graph = createGraph(nodes, DIRECTED);
	initialize_graph(dir_graph, pcap_nodes->head, pcap_nodes);
	
	remove_tree(pcap_nodes);
	printf("\nDIRECTED GRAPH\n");
	displayGraph(dir_graph);
	printf("\n");
	
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
	//printf("getting reachability of all nodes from a single point\n");
	find_reachable(dir_graph, 0, reach);
	
	for (i = 0; i < nodes; i++)
	{
		printf("reach[%d]=%d \n", i, reach[i]);
		if ( reach[i] == 0)
		{
			//printf("node = unreachable\n");
			//If node is unreachable, remove it from the graph -> add to BST.
			dead_zerg->head = removeNode(dir_graph, i, dead_zerg->head, &dead_zerg->count);
		}
	} 
	printf("\nUPDATED DIRECTED GRAPH");
	displayGraph(dir_graph);
	printf("\n");
	
	for ( i = 0; i < nodes; i++)
	{
		printf("Node [%d]: %d edges \n", i, dir_graph->adjListArr[i].num_members);
		if(  dir_graph->adjListArr[i].num_members < 2)
		{
			dead_zerg->head = removeNode(dir_graph, i, dead_zerg->head, &dead_zerg->count);
		}
	}
	
	printf("Number of nodes in removed_nodes: %d \n", dead_zerg->count );
	printf("Percentage: %.2f \n", (dead_zerg->count / (double)nodes) );
	
	if (  ( (dead_zerg->count / (double)nodes ) * 100) > 50.0)
	{
		printf("Removed more than 50%% of zergs : Not Zerg Connected \n");
		return 1;
	}
	
	printf("\nUPDATED DIRECTED GRAPH");
	displayGraph(dir_graph);
	
	
	//Identify a 
	//shortest_path(dir_graph, 0, 2, nodes);
	printf("\n");
	
	//void zerg_connected(graph_ptr graph, int start, int end, int num_nodes)
	zerg_connected(dir_graph, 0, 1, nodes);
	/*
	for ( int x = 0; x < nodes; x++)
	{
		for ( int y = 0; y < nodes; y++)
		{
			printf("starting at %d \n", x );
			zerg_connected(dir_graph, x, y, nodes);
		}
	}
	*/
	
	free(reach);
	destroyGraph(dir_graph);
	
	preOrder(dead_zerg->head, display_removed);
	remove_tree(dead_zerg);	
	return 0;
}

//Ensure all nodes are reachable from a given start point.
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

//Function to get haversine equation results.
double get_distance(double src_lat, double src_long, double src_alt, double dest_lat, double dest_long, double dest_alt)
{
	//printf("\n\nin get_distance\n");
	//printf("zerg_src->lat %f \n", src_lat );
	//printf("zerg_dest->lat %f \n", dest_lat );
	return haversine_dist(src_lat, src_long, src_alt, dest_lat, dest_long, dest_alt);
}

//Gather data from BST into the graph Adj. List Array
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

//From each point, attempt to add edge that meet minimum distance requirement.
void adding_nodes(graph_ptr graph, struct node * zerg, struct node * root)
{
	
	//printf("\n\nsrc\n");
	//printf("zerg_src->lat %f \n", ((struct zerg*)zerg->key)->position.latitude.value );
	//printf("zerg_src->long %f \n", ((struct zerg*)zerg->key)->position.longitude.value );
	
	double src_lat = ((struct zerg*)zerg->key)->position.latitude.value ;
	double src_long = ((struct zerg*)zerg->key)->position.longitude.value;
	double src_alt = ((struct zerg*)zerg->key)->position.altitude.value;
	int src_vertex = ((struct zerg*)zerg->key)->number;
	
	//printf("\n\ndest\n");
	//printf("zerg_dest->lat %f \n", ((struct zerg*)root->key)->position.latitude.value );
	//printf("zerg_dest->long %f \n", ((struct zerg*)root->key)->position.longitude.value );
	
	double dest_lat = ((struct zerg*)root->key)->position.latitude.value;
	double dest_long = ((struct zerg*)root->key)->position.longitude.value;
	double dest_alt = ((struct zerg*)root->key)->position.altitude.value;
	int dest_vertex = ((struct zerg*)root->key)->number;
	
	double distance = 0.00;
	if ( root != NULL)
	{
		//get_distance returns value in km -> multiply by 1000 to convert to meters.
		distance = ( get_distance(src_lat, src_long, src_alt, dest_lat, dest_long, dest_alt) * 1000);
		//If distance meets minimum distance ( 1.25 yds * 0.9144) converts to meters.
		if ( distance  > ( 1.25  * 0.9144)) 
		{
			//printf("distance = %f \n", distance);
			//If distance does not exceed max distance of 15 meters.
			if (distance > 15)
			{
				//printf("over 15 mdistance = %f \n", distance);
				return;
			}
			//printf("distance is withing range!!!! \n");
			addEdge(graph, src_vertex,((struct zerg*)zerg->key)->srcID, dest_vertex,((struct zerg*)root->key)->srcID, distance);
		}
	}
	return;
}

struct node * removeNode(graph_ptr graph, int node, struct node * root, int * count )
{
	//printf("inserting node %d to be removed\n", node);
	//Create a node for the node to be inserted
	root = insert(root, node, sizeof(int), get_root_srcID);
	(*count)++;
	//Assign the data to the node
	*((uint16_t*)(root->key)) = node;
	while ( graph->adjListArr[node].head != NULL)
	{
		//printf("edge vertex to del: [%d] \n",  graph->adjListArr[node].head->vertex);
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
	printf("zerg_alt: %06f \n",  ((struct zerg*)data)->position.altitude.value );
	printf("\n");
}
void display_removed(const void * data)
{
	printf("removing node: ");
	printf("zerg: [%u] \n", *((uint16_t*)(data)) );
}
