#ifndef TREE_H
#define TREE_H

typedef struct node {
	int nkeys;
	int key[3];
	struct node* parent;
	struct node* child[4];
} node;

node* insert(node* tree, int k);
node* search(node* tree, int k);
node* del_key(node* p, int k);

#endif