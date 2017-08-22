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
void display_removed(const void * data);
void get_health(struct node * root, int health);
void initialize_graph(graph_ptr graph, struct node * zerg, struct tree * pcaps);
void get_all_edges(graph_ptr graph, struct node * zerg, struct node * root);
void adding_nodes(graph_ptr graph, struct node * zerg, struct node * root);
double get_distance(double src_lat, double src_long, double src_alt, double dest_lat, double dest_long, double dest_alt);
void find_reachable(graph_ptr graph, const int start, bool reach[]);

void shortest_path( graph_ptr graph, int start_vertex, int graph_nodes );
void shortest_path_v2(graph_ptr graph, int start_node, int graph_nodes);

struct node * removeNode(graph_ptr graph, int node, struct node * root, int * count );

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
	
	//printf("pcap_nodes->head == NULL ? %c \n", pcap_nodes->head == NULL ? 'T':'F');
	//printf("\n\nprinting nodes\n\n");
	//preOrder(pcap_nodes->head, display_zerg);
	//get_health(pcap_nodes->head, min_health);  -> HOw to handle zergs with 0 hit points.
	
	graph_ptr dir_graph = createGraph(nodes, DIRECTED);
	initialize_graph(dir_graph, pcap_nodes->head, pcap_nodes);
	
	remove_tree(pcap_nodes);
	printf("\nDIRECTED GRAPH\n");
	//displayGraph(dir_graph);
	
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
	struct tree * removed_nodes = create_tree();
	removed_nodes->head = NULL;
	//printf("getting reachability of all nodes from a single point\n");
	find_reachable(dir_graph, 0, reach);
	
	for (i = 0; i < nodes; i++)
	{
		printf("reach[%d]=%d \n", i, reach[i]);
		if ( reach[i] == 0)
		{
			//printf("node = unreachable\n");
			//If node is unreachable, remove it from the graph -> add to BST.
			removed_nodes->head = removeNode(dir_graph, i, removed_nodes->head, &removed_nodes->count);
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
			removed_nodes->head = removeNode(dir_graph, i, removed_nodes->head, &removed_nodes->count);
		}
	}
	
	printf("Number of nodes in removed_nodes: %d \n", removed_nodes->count );
	printf("Percentage: %.2f \n", (removed_nodes->count / (double)nodes) );
	
	if (  ( (removed_nodes->count / (double)nodes ) * 100) > 50.0)
	{
		printf("Removed more than 50%% of zergs : Not Zerg Connected \n");
		return 1;
	}
	
	printf("\nUPDATED DIRECTED GRAPH");
	displayGraph(dir_graph);
	preOrder(removed_nodes->head, display_removed);
	
	/*
	printf("running dijkstras !!!! \n");
	shortest_path(dir_graph, 0, nodes);
	printf("\n");
	*/
	/*
	
	for ( int x = 0; x < nodes; x++)
	{
		for ( int y = 0; y < nodes y++)
		{
			printf("starting at %d \n", x );
			shortest_path_v2(dir_graph, x, nodes);
			// -> return a tu
		}
	}
	*/

	
	free(reach);
	destroyGraph(dir_graph);
	remove_tree(removed_nodes);	
	return 0;
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

/*
 Find two paths? 
	Use min heap with first run of dijkstra
	Use max heap with first run of dijkstra
	- This would get the shortest and longest path...remove edges of overlapping paths?

*/


// THe Dijkstra algorithm
void shortest_path( graph_ptr graph, int start_vertex, int graph_nodes)
{
	int v = graph_nodes; // number of vertex in the graph? pass in from BST?
	//Warning cleared -> Thanks to Dr. Oberts!!!
	double * distance = malloc(sizeof(double) * v);
	int * pred = malloc(sizeof(int) * graph_nodes);
	int i, j;
	
	//create a min heap
	struct MinHeap * minHeap = createMinHeap(v);
	printf("here\n");
	//initialize min heap with all vertices dist value of all vertex
	for (int x = 0; x < v ; ++x )
	{
		distance[x] = MAX;
		pred[x] = start_vertex;
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
				pred[v] = u;
				printf("v = %d \n", v);
				//UPDATE DISTANCE VALUE in min heap also
				decreaseKey(minHeap, v, distance[v]);
								
			}
			pCrawl = pCrawl->next;
		}
	}
	
	destroyHeap(minHeap);
	printf("\n");
	printArr(distance, v, start_vertex);
	printf("\n");
	for ( i = 0; i < graph_nodes; i++)
	{
		if (i != start_vertex)
		{
			printf("\n Distance to %d = %8.2f ", i , distance[i]);
			printf("     Path = %d ", i);
			j = i;
					
			do
			{
				j = pred[j];
				printf("<- %d ", j);
			}while (j != start_vertex);
		}
	}
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
