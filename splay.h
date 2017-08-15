struct node {
	void * key;
	struct node *left, *right;
};

struct tree {
	struct node * head;
	int count;
};

struct node * newNode(unsigned int key);
struct node * rightRotate(struct node * x);
struct node * leftRotate(struct node * x);
struct node * splay(struct node * root, uint16_t key, uint16_t (*get_srcID)(void * data));
struct node * search(struct node * root, uint16_t key, uint16_t (*get_srcID)(void * data));
struct node * insert(struct node * root, uint16_t key,  unsigned int size, uint16_t (*get_srcID)(void * data));
void preOrder(struct node * root, void (*display)(const void * data));
struct tree * create_tree(void);
