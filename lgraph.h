#ifndef _GRAPH_H_
#define _GRAPH_H_

typedef enum {UNDIRECTED = 0, DIRECTED} graph_type; //UNDIRECTED == 0 & DIRECTED == 1


/* Adj list node */
typedef struct adjlist_node
{
	int vertex;		
	double distance;					//INdex to adj list array.
	struct adjlist_node *next;			//Ptr of the next node
}adjlist_node, *adjlist_node_ptr;

/* Adj List */
typedef struct adjlist
{
	int num_members;					/* number of members in the list */
	adjlist_node * head;				/* head of adj list */
}adjlist, *adjlist_ptr;

/*Graph Structure*/
/*An array of adjacency lists. Size of array == number of vertices. */
typedef struct graph
{
	graph_type type;					/*Directed or Undirected*/
	int num_vertices;					/* number of vertices in graph*/
	adjlist_ptr adjListArr; 				/* adj. list array */
}graph, *graph_ptr;

/* Exit function */
__inline void err_exit(char * msg)
{
	printf("[Fatal Error]: %s \nExiting...\n", msg);
	exit(1);
}

adjlist_node_ptr createNode(int v, double distance);
graph_ptr createGraph(int n, graph_type type);
void destroyGraph(graph_ptr graph);
void addEdge(graph * graph, int src, int dest, double distance);
void displayGraph(graph_ptr graph);

#endif
