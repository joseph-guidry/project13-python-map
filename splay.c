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

void remove_tree(struct tree * old_tree)
{
	if ( old_tree->head != NULL )
	{
		delNode(old_tree->head);
		free(old_tree);
	}
}

//Makes new nodes with given key and NULL pointers.
struct node * newNode ( unsigned int size)
{
	//*build the zerg structure
	struct node * node = malloc(sizeof(*node));
	if ( node != NULL)
	{
		node->key = malloc(size);
		node->left = node->right = NULL;
		return (node);
	}
	return NULL;
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
	struct node * y = x->left;
	x->left = y->right;
	y->right = x;
	return y;
}

//Utility for a left rotate subtree rooted with x
struct node * leftRotate(struct node * x)
{
	struct node * y = x->right;
	x->right = y->left;
	y->left = x;
	return y;
}

struct node * splay(struct node * root, uint16_t key, uint16_t (*get_srcID)(void * data) )
{
	//Base cases: root = NULL or key at root
	if (root == NULL || get_srcID(root->key) == key)
	{
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
			root->left->left = splay(root->left->left, key, get_srcID);
			//First rotation
			root = rightRotate(root);
		}
		else if ( get_srcID(root->left->key) < key) //Left -Right -> else if less than
		{
			//Bring key to the root
			root->left->right = splay(root->left->right, key, get_srcID);
			
			if (root->left->right != NULL)
				root->left = leftRotate(root->left);
		}
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
			root->right->left = splay(root->right->left, key, get_srcID);
			//First rotation of right
			if ( root->right->left != NULL)
				root->right = rightRotate(root->right);
		}	
		else if ( get_srcID(root->right->key) < key)
		{
			//Bring key of right right 
			root->right->right = splay(root->right->right, key, get_srcID);
			root = leftRotate(root);
		}
		return ( root->right == NULL) ? root: leftRotate(root);
	}
}

struct node * search_srcID(struct node * root, uint16_t key, uint16_t (*get_srcID)(void * data))
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

struct node * insert(struct node * root, uint16_t src, unsigned int size, uint16_t (*get_srcID)(void * data) )
{

	//Insertion to empty tree
	if (root == NULL) 
		return newNode(size);
	
	//Find node if present
	root = splay(root, src, get_srcID);
	uint16_t src_ID = get_srcID(root->key);
	//If found
	if ( src_ID == src) 
		return root;
	struct node * newnode = newNode(size);
	
	//Make root as right child of newnode -> copy left child of root to newNode
	if ( src_ID > src)
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

#endif

