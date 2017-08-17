/*   MIN HEAP FUNCTIONS */
// Used from geeksforgeeks.org //
struct MinHeapNode
{
	int v;
	uint16_t src_ID; //Only needed for printing.
	double distance;
};

struct MinHeap
{
	int size;
	int capacity;
	int * pos;
	struct MinHeapNode **array;
};

struct MinHeapNode * newMinHeapNode(int v, uint16_t src_ID, double distance);
struct MinHeap * createMinHeap(int capacity);
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b);
void minHeapify(struct MinHeap * minHeap, int idx);
int isEmpty(struct MinHeap * minHeap);
struct MinHeapNode * extractMin(struct MinHeap * minHeap);
void decreaseKey(struct MinHeap * minHeap, int v, double distance);
bool isInMinHeap(struct MinHeap * minHeap, int v);
void printArr(double distance[], int n, int start_vertex);

void destroyNode(struct MinHeapNode * minHeapNode);
void destroyHeap(struct MinHeap * minHeap);
