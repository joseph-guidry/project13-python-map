#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lgraph.h"


#define MAX 1000
void shortest_path_v1( graph_ptr graph, int start_vertex);
void shortest_path_v2(graph_ptr graph, int startnode, int graph_nodes);

int compare_paths(int * path1, int * path2, int num_nodes, int start, int end)
{
	for ( int x = 0; x < num_nodes; x++)
	{
		for ( int y = 0; y < num_nodes; y++)
		{
			if ( path1[x] == path2[y] )
			{
				if ( path1[x] == start || path1[x] == end )
					break;
				else
					return path1[x];
			}
		}
	}
	return -1;
}

int * find_shortest_path(graph_ptr graph, int start, int end, int * reach, int * path, int count)
{
	reach[start] = 1;
	int vertex = start, shortest = 100;
	//printf("start: %d | end: %d | count: %d\n", start, end, count );
	//printf("next vertex: [%d] \n", start);
	printf("path[x] = %d \n", vertex);
	path[count++] = vertex;
	
	if ( start == end )
		return path;
		
	
	
	struct adjlist_node * pCrawl = graph->adjListArr[start].head;
	while( pCrawl != NULL)
	{
		//printf("here1 [%d]\n", path[pCrawl->vertex]);
		if ( !reach[pCrawl->vertex])
		{
			//printf("here2\n");
			if ( shortest > pCrawl->distance)
			{
				//printf("here3\n");
				shortest = pCrawl->distance;
				vertex = pCrawl->vertex;
			}
		}
		pCrawl = pCrawl->next;
	}
	
	find_shortest_path(graph, vertex, end, reach, path, count);
	return path;
}


int * find_longest_path(graph_ptr graph, int start, int end, int * reach, int * path, int count)
{
	reach[start] = 1;
	int vertex = start, longest = 0;
	printf("start: %d | end: %d | count: %d\n", start, end, count );
	printf("next vertex: [%d] \n", start);
	printf("path[x] = %d \n", vertex);
	path[count++] = vertex;
	
	
	// If you reach the end of the list and find a match
	if ( start == end )
		return path;
	// If you exceed the size of the amount of nodes in the array, return invalid path.
	//printf("count:%lu \n", (sizeof(path)/sizeof(int)) );
	if ( (unsigned)count > 8 )
	{
		printf("OR exiting here: %d\n", count);
		return path;
	}
	
	
	struct adjlist_node * pCrawl = graph->adjListArr[vertex].head;
	//printf("head->vertex: %d \n", pCrawl->vertex);
	//printf("\n\t %d\n\n ", graph->adjListArr[start].num_members );
	if( graph->adjListArr[start].num_members < 3)
	{
		while( pCrawl != NULL)
		{
			//printf("here1 [%d]:[%d]\n", pCrawl->vertex, count);
			if ( !reach[pCrawl->vertex] )
			{
				//if ( !reach[pCrawl->next->vertex] )
				if ( longest < pCrawl->distance)
				{
					longest = pCrawl->distance;
					vertex = pCrawl->vertex;
				}
			}
			pCrawl = pCrawl->next;
		}
		//printf("pCrawl == NULL? %c \n", pCrawl == NULL ? 'T':'F');
		find_longest_path(graph, vertex, end, reach, path, count);
	}
	else
	{
		printf("greater than 3\n");
		
		while( pCrawl != NULL)
		{
			//printf("HERE!!! [%d]:[%d]\n", pCrawl->vertex, pCrawl->next->vertex);
			if ( !reach[pCrawl->vertex]  )
			{
				if ( !reach[pCrawl->next->vertex]  )
				{
					//Attempt to avoid a path that dead ends...
				}
				else if ( longest < pCrawl->distance)
				{
					// TODO
				}
			}
			
			pCrawl = pCrawl->next;
		}
		printf("pCrawl == NULL? %c \n", pCrawl == NULL ? 'T':'F');
		find_longest_path(graph, vertex, end, reach, path, count);
	
	}
	return path;
}


void zerg_connected(graph_ptr graph, int start, int end, int num_nodes)
{
//void removeEdge(graph * graph, int src, int dest);
	printf("Values: %d %d %d \n", start, end, num_nodes);
	
	printf("here in zerg_connect\n");
	int count = 0;
	int * reach = NULL, * path1 = NULL;
	reach = malloc(sizeof(int) * num_nodes );
	path1 = malloc(sizeof(int) * num_nodes );
	for ( int x = 0; x < num_nodes; x++)
	{
		reach[x] = 0;
		path1[x] = 0;
	}
	printf("getting shortest path\n");
	path1 = find_shortest_path(graph, start, end, reach, path1, count);
	
	for ( int x = 0; x < num_nodes; x++ )
	{
		printf("[%d] ", path1[x]);
		
		printf("remove edge: %d to %d \n", path1[x], path1[x + 1]);
		removeEdge(graph, path1[x], path1[x + 1]);
		if ( path1[x + 1] == end )
			break;
		
	} 
	int * path2 = NULL;
	path2 = malloc(sizeof(int) * num_nodes );
	// Initialize values in variables to 0
	for ( int x = 0; x < num_nodes; x++)
	{
		reach[x] = 0;
		path2[x] = 0;
	}
	displayGraph(graph); 
	printf("getting longest path\n");
	find_longest_path(graph, start, end, reach, path2, count);
	
	for ( int x = 0; x < num_nodes; x++ )
	{
		printf("[%d] ", path2[x]);
		
		//printf("remove edge: %d to %d \n", path2[x], path2[x + 1]);
		if ( path2[x + 1] == end )
			break;
		
	} 
	printf("here about to cmp-paths\n");
	int results = compare_paths(path1, path2, num_nodes, start, end);	

	if ( (results < 0) )
	{
		printf("Graph is zerg connected\n");
	}
	else
	{
		printf("Zerg %d needs to be removed \n", results);
	}
	
}

