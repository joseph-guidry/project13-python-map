#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "min_heap.h"

//A utility funciton to create a new Min Heap Node
struct MinHeapNode * newMinHeapNode(int v, uint16_t src_ID, double distance)
{
	struct MinHeapNode * minHeapNode = (struct MinHeapNode *)malloc(sizeof(struct MinHeapNode));
	minHeapNode->v = v;
	minHeapNode->src_ID = src_ID;
	minHeapNode->distance = distance;
	return minHeapNode;
}

void destroyHeapNode(struct MinHeapNode * minHeapNode)
{
	free(minHeapNode);
}

// Utility to make a min heap -> where does capacity come from.
struct MinHeap * createMinHeap(int capacity)
{
	struct MinHeap * minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
	minHeap->pos = (int *)malloc(sizeof(int) * capacity);
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array = (struct MinHeapNode**)malloc(capacity * sizeof(struct MinHeapNode * ));
	return minHeap;
}

void destroyHeap(struct MinHeap * minHeap)
{
	for (int x = 0; x < minHeap->size; x++)
	{
		destroyHeapNode(minHeap->array[x]);
	}
	free(minHeap->pos);
	free(minHeap);
}

//Swap nodes in heap
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode ** b)
{
	struct MinHeapNode * temp = *a;
	*a = *b;
	*b = temp;
}

// Heapify function at given index
// Function also updates position of nodes when swapped.
// Position is needed for decreaseKey()
void minHeapify(struct MinHeap * minHeap, int idx)
{
	int smallest, left, right;
	smallest = idx,
	left = 2 *idx + 1;
	right = 2 * idx + 2;
	
	if (left < minHeap->size && minHeap->array[left]->distance < minHeap->array[smallest]->distance)
	{
		smallest = left;
	}
	if ( right < minHeap->size && minHeap->array[right]->distance < minHeap->array[smallest]->distance)
	{
		smallest = right;
	}
	if ( smallest != idx )
	{
		//The nodes to be swapped in min heap
		struct MinHeapNode * smallestNode = minHeap->array[smallest];
		struct MinHeapNode * idxNode = minHeap->array[idx];

		//Swap positions
		minHeap->pos[smallestNode->v] = idx;
		minHeap->pos[idxNode->v] = smallest;
		
		//Swap nodes
		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
		
		minHeapify(minHeap, smallest);		
	}
}

// Is heap empty check
int isEmpty(struct MinHeap * minHeap)
{
	return minHeap->size == 0;
}

//extract min node from the heap
struct MinHeapNode * extractMin(struct MinHeap * minHeap)
{
	if ( isEmpty(minHeap) )
		return NULL;
	
	//Store the root node
	struct MinHeapNode * root = minHeap->array[0];
	
	//Replace root with last node
	struct MinHeapNode * lastNode = minHeap->array[minHeap->size - 1];
	minHeap->array[0] = lastNode;
	
	//Update position of last node
	minHeap->pos[root->v] = minHeap->size-1;
	minHeap->pos[lastNode->v] = 0;
	
	//Reduce heap size and heap root
	--minHeap->size;
	minHeapify(minHeap, 0);
	
	return root;
}

//Decrease distance value of a given vertex v.
void decreaseKey(struct MinHeap * minHeap, int v, double distance)
{
	//Get index of v in heap array
	int i = minHeap->pos[v];
	
	//Get node and update the distance value
	minHeap->array[i]->distance = distance;
	
	//Travel up while the complete tree is not heapified
	// O(log N) loop
	while ( i && minHeap->array[i]->distance < minHeap->array[(i-1)/2]->distance)
	{
		minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
		minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
		
		swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i-1)/2]);
		
		//move to parent index
		i = (i - 1) / 2;
	}
}

// Check if vertex is in the heap

bool isInMinHeap(struct MinHeap * minHeap, int v)
{
	if ( minHeap->pos[v] < minHeap->size)
		return true;
	return false;
}

//printing the path solution
void printArr(double distance[], int n, int start_vertex)
{
	printf("Vertex	Distance from source \n");
	for (int i = 0; i < n ; i++)
	{
		printf("From %d to %d \t %f\n", start_vertex, i, distance[i]);
	}
}
