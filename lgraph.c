#include <stdio.h>
#include <stdlib.h>

#include "lgraph.h"


#ifndef _LGRAPH_H_
#define _LGRAPH_H_
/* credit to the Internet */
/* Function to create adj. list node*/
adjlist_node_ptr createNode(int v, uint16_t src_id, double distance)
{
	//printf("creating new node\n");
	adjlist_node_ptr newNode = (adjlist_node_ptr)malloc(sizeof(adjlist_node));
	if(!newNode)
		//err_exit("Unable to allocate memory for new node");
		fprintf(stderr, "Unable to allocate memory for new node");
		
	newNode->vertex = v;
	newNode->src_ID = src_id;
	newNode->distance = distance;
	newNode->next = NULL;
	
	return newNode;
}

/* Create funciton to destroy the old node */
void destroyNode(adjlist_node_ptr old_node)
{
	free(old_node);
}

/* Function to greate a graph with N vertices; creates both types of graphs(Undirected and directed)*/
graph_ptr createGraph(int n, graph_type type)
{
	int i;
	graph_ptr graph = (graph_ptr)malloc(sizeof(graph));
	if(!graph)
		//err_exit(
		fprintf(stderr, "Unable to allocate memory for the graph");
	graph->num_vertices = n;
	graph->type = type;
	
	//Create adj. list array
	graph->adjListArr = (adjlist_ptr)malloc(n * sizeof(adjlist));
	if (!graph->adjListArr)
		fprintf(stderr, "Unable to allocate memory for adj.list array\n");
		
	for(i = 0; i < n; i++)
	{
		graph->adjListArr[i].head = NULL;
		graph->adjListArr[i].num_members = 0;
	}
	
	return graph;
}

void destroyGraph(graph_ptr graph)
{
	if(graph)
	{
		if(graph->adjListArr)
		{
			int v;
			//Free up the nodes in list
			for (v = 0; v < graph->num_vertices; v++)
			{
				adjlist_node_ptr adjListPtr = graph->adjListArr[v].head;
				while( adjListPtr)
				{
					adjlist_node_ptr tmp = adjListPtr;
					adjListPtr = adjListPtr->next;
					free(tmp);
				}
			}
			//Free adj list
			free(graph->adjListArr);
		}
		//Free graph
		free(graph);
	}
}

/* Add edge to graph */
void addEdge(graph * graph, int src, uint16_t src_id, int dest, uint16_t dest_id, double distance)
{
	//printf("adding edge\n");
	//printf("src: %u | dest: %u \n", src, dest);
	//printf("dist: %f \n", distance);
	/*add an edge from src to dst in the adj. list*/
	adjlist_node_ptr current;
	adjlist_node_ptr newNode = createNode(dest, dest_id, distance);
	if ( graph->adjListArr[src].num_members == 0 || graph->adjListArr[src].head->vertex >= newNode->vertex)
	{
		newNode->next = graph->adjListArr[src].head;
		graph->adjListArr[src].head = newNode;
	}
	else
	{
		current = graph->adjListArr[src].head;
		while( current->next != NULL && current->next->vertex < newNode->vertex )
		{
			current = current->next;
		}
		newNode -> next = current->next;
		current->next = newNode;
	}	
	graph->adjListArr[src].num_members++;
	
	if(graph->type == UNDIRECTED)
	{
		//add edge going the other direction as well
		newNode = createNode(src, src_id, distance);
		newNode->next = graph->adjListArr[dest].head;
		graph->adjListArr[dest].head = newNode;
		graph->adjListArr[dest].num_members++;
	}
	
}


// Made to remove nodes that have only 1 edge -> add to the removed_zerg BST
// Remove edges from dijstra shortest path -> add to the tree
void removeEdge(graph * graph, int src, int dest)
{
	adjlist_node_ptr current, previous;
	
	current = graph->adjListArr[src].head;
	//printf("in Remove Edge \n");
	// Remove the first / only node in the list
	if ( current->next == NULL)
	{
		//printf("destroy only node in the lsit\n");
		graph->adjListArr[src].head = graph->adjListArr[src].head->next;
		destroyNode(current);
		graph->adjListArr[src].num_members--;
		return;
	}
	// Traverse until the node to delete is found
	while ( (current != NULL) && (current->vertex != dest) )
	{
		//printf("The current node is %d \n", current->vertex);
		previous = current;
		current = current->next;
	}
	//printf("after while: current = %c \n", current == NULL ? 'T':'F');
	if ( current == NULL )
	{
		printf("Not found in the list \n");
	}
	else
	{
		// if dest is found and at the head of the list
		if ( graph->adjListArr[src].head == current )
		{
			graph->adjListArr[src].head = current->next;
			destroyNode(current);
		}
		// if dest is found and in the middle/end of the list
		else
		{
			//printf("trying node in linked list\n");
			previous->next = current->next == NULL ? NULL: current->next;
			//printf("here\n");
			destroyNode(current);
		}
	}
	graph->adjListArr[src].num_members--;
	return;
}

/* Graph display function */
void displayGraph(graph_ptr graph)
{
	int i;
	for (i = 0; i < graph->num_vertices; i++)
	{
		adjlist_node_ptr adjListPtr = graph->adjListArr[i].head;
		printf("\n%d: ", i);
		while ( adjListPtr)
		{
			printf("%d:%04u:%0.2f-> ", adjListPtr->vertex, adjListPtr->src_ID, adjListPtr->distance);
			adjListPtr = adjListPtr->next;
		}
		printf("NULL\n");
	}
}
#endif


