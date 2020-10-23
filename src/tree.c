#include "tree.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

node *search(node *tree, int k)
{
    if (!tree)
        return NULL;
    int found = 0, i;
    for (i = 0; i < tree->nkeys; i++)
    {
        if (tree->key[i] == k)
        {
            found = 1;
            break;
        }
    }
    if (found)
        return tree;
    else if (k < tree->key[0])
        return search(tree->child[0], k);
    else if ((tree->nkeys == 2 && k < tree->key[1]) || (tree->nkeys == 1))
        return search(tree->child[1], k);
    else if (tree->nkeys == 2)
        return search(tree->child[2], k);
    return NULL;
}

int is_leaf(node *tree)
{
    return (tree->child[0] == NULL) && (tree->child[1] == NULL) && (tree->child[2] == NULL);
}

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void sort(node *tree)
{
    if (tree->nkeys == 1)
    {
        return;
    }
    if (tree->nkeys == 2)
    {
        if (tree->key[0] > tree->key[1])
        {
            swap(&tree->key[0], &tree->key[1]);
        }
    }
    else
    {
        if (tree->key[0] > tree->key[1])
        {
            swap(&tree->key[0], &tree->key[1]);
        }
        if (tree->key[0] > tree->key[2])
        {
            swap(&tree->key[0], &tree->key[2]);
        }
        if (tree->key[1] > tree->key[2])
        {
            swap(&tree->key[1], &tree->key[2]);
        }
    }
}

void insert_to_node(node *tree, int k)
{
    tree->key[tree->nkeys++] = k;
    sort(tree);
}

node *split(node *tree)
{
    node *x = (node *)malloc(sizeof(node));
    node *y = (node *)malloc(sizeof(node));
    if (!x || !y)
    {
        printf("Can't allocate memory!");
        return NULL;
    }
    for (int i = 0; i < 2; i++)
    {
        x->child[i] = tree->child[i];
        y->child[i] = tree->child[i + 2];
        if (x->child[i])
            x->child[i]->parent = x;
        if (y->child[i])
            y->child[i]->parent = y;
        x->child[i + 2] = NULL;
        y->child[i + 2] = NULL;
    }
    x->nkeys = 1;
    x->key[0] = tree->key[0];
    x->parent = tree->parent;
    y->nkeys = 1;
    y->key[0] = tree->key[2];
    y->parent = tree->parent;
    if (tree->parent)
    {
        insert_to_node(tree->parent, tree->key[1]);
        for (int i = 0; i < 3; i++)
        {
            if (tree->parent->child[i] == tree)
            {
                tree->parent->child[i] = NULL;
                break;
            }
        }
        int j = 0;
        while (tree->parent->child[j] != NULL && j < 2)
            j++;
        for (int i = 3; i >= j + 2; i--)
            tree->parent->child[i] = tree->parent->child[i - 1];
        tree->parent->child[j + 1] = y;
        tree->parent->child[j] = x;
        node *tmp = tree->parent;
        free(tree);
        return tmp;
    }
    else
    {
        x->parent = tree;
        y->parent = tree;
        tree->key[0] = tree->key[1];
        tree->nkeys = 1;
        tree->child[0] = x;
        tree->child[1] = y;
        tree->child[2] = NULL;
        tree->child[3] = NULL;
        return tree;
    }
}

node *insert(node *tree, int k)
{
    if (!tree)
    {
        tree = (node *)malloc(sizeof(node));
        if (!tree)
        {
            fprintf(stderr, "Can't allocate memory!");
            return NULL;
        }
        tree->nkeys = 1;
        tree->key[0] = k;
        tree->parent = NULL;
        int i;
        for (i = 0; i < 4; i++)
        {
            tree->child[i] = NULL;
        }
        return tree;
    }
    if (is_leaf(tree))
    {
        insert_to_node(tree, k);
    }
    else if (k < tree->key[0])
    {
        insert(tree->child[0], k);
    }
    else if ((tree->nkeys == 1) || ((tree->nkeys == 2) && (k < tree->key[1])))
    {
        insert(tree->child[1], k);
    }
    else
    {
        insert(tree->child[2], k);
    }
    if (tree->nkeys <= 2)
    {
        return tree;
    }
    return split(tree);
}