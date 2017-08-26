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
	//printf("path[x] = %d \n", vertex);
	path[count++] = vertex;
	
	if ( start == end )
		return path;
		
	
	
	struct adjlist_node * pCrawl = graph->adjListArr[start].head;
	while( pCrawl != NULL)
	{
		//printf("here1 [%d]\n", reach[pCrawl->vertex]);
		if ( !reach[pCrawl->vertex])
		{
			if ( shortest > pCrawl->distance || (pCrawl->vertex == end) )
			{
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
	int prev = start;
	int vertex = start, longest = 0;
	//printf("start: %d | end: %d | count: %d\n", start, end, count );
	//("next vertex: [%d] \n", start);
	//printf("path[x] = %d \n", vertex);
	path[count++] = vertex;
	
	
	// If you reach the end of the list and find a match
	if ( start == end )
		return path;
	// If you exceed the size of the amount of nodes in the array, return invalid path.
	//printf("count:%lu \n", (sizeof(path)/sizeof(int)) );
	if ( (unsigned)count > 8 )
	{
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
		//printf("greater than 3\n");
		struct adjlist_node * pCrawler = pCrawl;
		//Sweep through the list to idenify node with least amount of edges.
		while( pCrawl != NULL)
		{
			//printf("HERE!!! [%d]:[%d]\n", pCrawl->vertex, reach[pCrawl->vertex]);
			if ( !reach[pCrawl->vertex]  )
			{
				//printf("Number of edges %d \n", graph->adjListArr[pCrawl->vertex].num_members);
				
				if ( graph->adjListArr[pCrawl->vertex].num_members == 2 )
				{
					
					longest = pCrawl->distance;
					vertex = pCrawl->vertex;
				}
			}
			
			pCrawl = pCrawl->next;
		}
		//If edges are all higher than 2, use the next node of the longest distance.
		
		if (vertex == prev)
		{
			while( pCrawler != NULL)
			{
				//printf("HERE!!! [%d]:[%d]\n", pCrawler->vertex, reach[pCrawler->vertex]);
				if ( !reach[pCrawler->vertex]  )
				{
					//printf("Number of edges %d \n", graph->adjListArr[pCrawler->vertex].num_members);
					if ( longest < pCrawler->distance)
					{
						vertex = pCrawler->vertex;
						longest = pCrawler->distance;
					
					}
				}
			
				pCrawler = pCrawler->next;
			}
		}
		
		//printf("pCrawl == NULL? %c \n", pCrawl == NULL ? 'T':'F');
		//printf("prev %d : %d vertex \n", prev, vertex);
		if ( !(prev == vertex) )
			find_longest_path(graph, vertex, end, reach, path, count);
	
	}
	//printf("could not find path\n");
	return path;
}

int * zerg_connected(graph_ptr graph, int start, int end, int num_nodes)
{
	int x, count = 0;
	int * reach = NULL, * path1 = NULL;
	reach = malloc(sizeof(int) * num_nodes );
	path1 = malloc(sizeof(int) * num_nodes );
	
	for ( x = 0; x < num_nodes; x++)
	{
		reach[x] = 0;
		path1[x] = 0;
	}
	
	path1 = find_shortest_path(graph, start, end, reach, path1, count);
	for ( x = 0; x < num_nodes; x++ )
	{
		//printf("[%d] ", path1[x]);
		removeEdge(graph, path1[x], path1[x + 1]);
		//opposite edge and replace with weight of -1?
		removeEdge(graph, path1[x + 1], path1[x]);
		addEdge(graph, path1[x+1],  graph->adjListArr[path1[x+1]].src_ID, path1[x], graph->adjListArr[path1[x]].src_ID, -1.00);
		
		if ( path1[x + 1] == end )
		{
			//printf("[%d]", path1[x + 1]);
			break;
		}
	} 
	printf("\n");
	/*
	printf("UPDATED after removing original path GRAPH\n");
	displayGraph(graph);
	printf("\n");
	*/
	int * path2 = NULL;
	path2 = malloc(sizeof(int) * num_nodes );
	// Initialize values in variables to 0
	for ( int x = 0; x < num_nodes; x++)
	{
		reach[x] = 0;
		path2[x] = 0;
	}
	 
	path2 = find_longest_path(graph, start, end, reach, path2, count);
	for ( x = 0; x < num_nodes; x++ )
	{
		//printf("[%d] ", path2[x]);
		if ( path2[x + 1] == end )
		{
			//printf("[%d]", path2[x + 1]);
			break;
		}
		
	} 
	//printf("\n");
	int idx, end_idx, results = compare_paths(path1, path2, num_nodes, start, end);	
	int * remove = NULL;
	if ( (results < 0) )
	{
		printf("Zergs are fully connected\n");
	}
	else
	{
		for ( x = 0; x < num_nodes; x++)
		{
			if ( path1[x] == results)
				idx = x;
			
			if ( path1[x] == end)
				end_idx = x;
		}
		if ( idx > (end_idx / 2) )
		{
			printf("Zergs after %d needs to be removed \n", results);
			remove = malloc(sizeof(int) * num_nodes);
			count = 0;;
			printf("Zergs after %d needs to be removed \n", results);
			for ( int x = 0; x < num_nodes; x++)
			{
				printf("vertex: %d\n", path1[x]);
				if ( path1[x] == results)
				{
					for ( int y = x + 1; path1[y] != -1; y++)
					{
						printf("remove vertex :%d\n", path1[y] );
						remove[count++] = path1[y];
					}
					break;
				}
			}
			for ( int x = 0; x < num_nodes; x++)
			{
				printf("vertex: %d\n", path1[x]);
				if ( path2[x] == results)
				{
					for ( int y = x + 1; path2[y] != end; y++)
					{
						printf("remove vertex :%d\n", path2[y] );
						remove[count++] = path2[y];
					}
				}
			}
		}
		else
		{
			printf("Zergs before %d needs to be removed \n", results);
			remove = malloc(sizeof(int) * num_nodes);
			count = 0;
			for ( x = 0; x < num_nodes; x++)
			{
				printf("vertex: %d \n", path1[x]);
				if ( path1[x] == results)
					break;
				remove[count++] = path1[x];
			}
			for ( x = 0; x < num_nodes; x++)
			{
				printf("vertex: %d \n", path2[x]);
				if ( path2[x] == start)
					continue;
				if ( path2[x] == results)
					break;
				remove[count++] = path2[x];
			}
		}
	}
	
	free(reach);
	free(path1);
	free(path2);
	return remove;
}

