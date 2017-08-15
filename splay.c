#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>

#include "splay.h"

#ifndef _SPLAY_H_
#define _SPLAY_H_

struct tree * create_tree(void)
{
	struct tree * tree = malloc(sizeof(struct tree));
	
	tree->head = NULL;
	tree->count = 0;
	return tree;
}

//Makes new nodes with given key and NULL pointers.
struct node * newNode ( unsigned int size)
{
	//*build the zerg structure
	//printf("making a node with size = %d \n", size);
	struct node * node = (struct node*)malloc(sizeof(struct node));
	node->key = malloc(size);
	node->left = node->right = NULL;
	return (node);
}

void delNode(struct node * root)
{
	if( root != NULL)
	{
		delNode(root->left);
		delNode(root->right);
		free(root->key);
		free(root);
	}
}

//Utility to right rotate subtre root with y//
struct node * rightRotate(struct node *x)
{	
	//printf("in rightRotate\n");
	struct node * y = x->left;
	x->left = y->right;
	y->right = x;
	return y;
}

//Utility for a left rotate subtree rooted with x
struct node * leftRotate(struct node * x)
{
	//printf("in leftRotate\n");
	struct node * y = x->right;
	x->right = y->left;
	y->left = x;
	//printf("returning y\n");
	return y;
}

struct node * splay(struct node * root, uint16_t key, uint16_t (*get_srcID)(void * data) )
{
	//printf("in splay \n");
	//Base cases: root = NULL or key at root
	if (root == NULL || get_srcID(root->key) == key)
	{
		//printf("root == NULL?\n");
		/* root == searched value */ 
		/* modify data */
		return root;
	}
	
	//Look in left subtree
	if (get_srcID(root->key) > key)
	{
		if ( root->left == NULL )
			return root;
		
		//Left - Left
		if ( get_srcID(root->left->key) > key) // if greater than
		{
			//Bring key to the root
			//printf("doing splay left left\n");
			root->left->left = splay(root->left->left, key, get_srcID);
			//First rotation
			root = rightRotate(root);
		}
		else if ( get_srcID(root->left->key) < key) //Left -Right -> else if less than
		{
			//Bring key to the root
			//printf("doing splay left right\n");
			root->left->right = splay(root->left->right, key, get_srcID);
			
			if (root->left->right != NULL)
				root->left = leftRotate(root->left);
		}
		//printf("here in splay1\n");
		return (root->left == NULL) ? root : rightRotate(root);
	}	
	else //Lookin in right subtree
	{
		//Not in the tree-> DONE
		if ( root->right == NULL)
			return root;
		//Right Left
		if ( get_srcID(root->right->key) > key)
		{
			//bring up key of right left
			//printf("doing splay right left\n");
			root->right->left = splay(root->right->left, key, get_srcID);
			//First rotation of right
			if ( root->right->left != NULL)
				root->right = rightRotate(root->right);
		}	
		else if ( get_srcID(root->right->key) < key)
		{
			//Bring key of right right 
			//printf("doing splay right right\n");
			root->right->right = splay(root->right->right, key, get_srcID);
			//printf("back here\n");
			root = leftRotate(root);
		}
		//printf("here in splay2\n");
		return ( root->right == NULL) ? root: leftRotate(root);
	}
}

struct node * search(struct node * root, uint16_t key, uint16_t (*get_srcID)(void * data))
{
	return splay(root, key, get_srcID );
}

void preOrder(struct node * root, void (*display)(const void * data))
{
	if (root != NULL)
	{
		display(root->key);
		preOrder(root->left, display);
		preOrder(root->right, display);
	}
}

struct node * insert(struct node *root, uint16_t src, unsigned int size, uint16_t (*get_srcID)(void * data) )
{

	//printf("inserting src: %u \n", src);
	//Insertion to empty tree
	if (root == NULL) 
		return newNode(size);
	
	//Find node if present
	root = splay(root, src, get_srcID);
	//printf("return from splaying\n");
	//If found
	if ( get_srcID(root->key) == src) 
		return root;
	//printf("here after splaying node\n");
	struct node * newnode = newNode(src);
	
	//Make root as right child of newnode -> copy left child of root to newNode
	if ( get_srcID(root->key) > src)
	{
		newnode->right = root;
		newnode->left = root->left;
		root->left = NULL;
	}
	//make root as left child of newNode-> copy right child of root to newNode
	else
	{
		newnode->left = root;
		newnode->right = root->right;
		root->right = NULL;
	}
	return newnode;
}
/*
int main(void)
{
	
	struct node * root = newNode(100);
	root->left = newNode(50);
	root->right = newNode(200);
	root->left->left = newNode(40);
	root->left->left->left = newNode(30);
	root->left->left->left->left = newNode(20);
	//root = search(root, 20);
	*/
	/*
	unsigned int randval;
	FILE * fptr;
	
	fptr = fopen("/dev/random", "r");
	fread(&randval, sizeof(randval), 1, fptr);
	fclose(fptr);
	
	printf("randval %u\n", randval);
	
	struct node * root = NULL;
	//root = insert(root, randval );
	//srand(time(NULL));
	for(int x = 0; x < 10; ++x)
	{
		printf("here\n");
		root = insert(root, x *x *x + 5);
	}
	root = search(root, 5);
	
	preOrder(root);
	printf("\n");
}
*/
#endif

