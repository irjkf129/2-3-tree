#ifndef TREE_H
#define TREE_H

typedef struct btree {
	int nkeys;
	int key[3];
	struct btree* parent;
	struct btree* child[4];
} btree;


btree* insert(btree* tree, int k);
btree* search(btree* tree, int k);
btree* del_key(btree* p, int k);
btree *search_max(btree *p);
btree *search_min(btree *p);

#endif