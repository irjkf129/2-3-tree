#include "tree.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

btree *search(btree *tree, int k)
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

int is_leaf(btree *tree)
{
    return (tree->child[0] == NULL) && (tree->child[1] == NULL) && (tree->child[2] == NULL);
}

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void sort(btree *tree)
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

void insert_to_node(btree *tree, int k)
{
    tree->key[tree->nkeys++] = k;
    sort(tree);
}

btree *split(btree *tree)
{
    btree *x = (btree *)malloc(sizeof(btree));
    btree *y = (btree *)malloc(sizeof(btree));
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
        btree *tmp = tree->parent;
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

btree *insert(btree *tree, int k)
{
    if (!tree)
    {
        tree = (btree *)malloc(sizeof(btree));
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

btree *search_min(btree *p)
{
    if (!p)
        return p;
    if (!(p->child[0]))
        return p;
    else
        return search_min(p->child[0]);
}

btree *search_max(btree *p)
{
    if (!p)
        return p;
    int m = 1;
    if (p->nkeys == 2)
        m++;
    if (!(p->child[m]))
        return p;
    else
        return search_max(p->child[m]);
}

void remove_from_node(btree *p, int k)
{
    if (p->nkeys >= 1 && p->key[0] == k)
    {
        p->key[0] = p->key[1];
        p->key[1] = p->key[2];
        p->nkeys--;
    }
    else if (p->nkeys == 2 && p->key[1] == k)
    {
        p->key[1] = p->key[2];
        p->nkeys--;
    }
}

btree *redistribute(btree *leaf)
{
    btree *parent = leaf->parent;
    btree *child[3] = {parent->child[0], parent->child[1], parent->child[2]};
    if ((parent->nkeys == 2) && (child[0]->nkeys < 2) && (child[1]->nkeys < 2) && (child[2]->nkeys < 2))
    {
        if (child[0] == leaf)
        {
            parent->child[0] = parent->child[1];
            parent->child[1] = parent->child[2];
            parent->child[2] = NULL;
            insert_to_node(parent->child[0], parent->key[0]);
            parent->child[0]->child[2] = parent->child[0]->child[1];
            parent->child[0]->child[1] = parent->child[0]->child[0];
            if (leaf->child[0] != NULL)
                parent->child[0]->child[0] = leaf->child[0];
            else if (leaf->child[1] != NULL)
                parent->child[0]->child[0] = leaf->child[1];
            if (parent->child[0]->child[0] != NULL)
                parent->child[0]->child[0]->parent = parent->child[0];
            remove_from_node(parent, parent->key[0]);
            free(child[0]);
        }
        else if (child[1] == leaf)
        {
            insert_to_node(child[0], parent->key[0]);
            remove_from_node(parent, parent->key[0]);
            if (leaf->child[0] != NULL)
                child[0]->child[2] = leaf->child[0];
            else if (leaf->child[1] != NULL)
                child[0]->child[2] = leaf->child[1];

            if (child[0]->child[2] != NULL)
                child[0]->child[2]->parent = child[0];

            parent->child[1] = parent->child[2];
            parent->child[2] = NULL;

            free(child[1]);
        }
        else if (child[2] == leaf)
        {
            insert_to_node(child[1], parent->key[1]);
            parent->child[2] = NULL;
            remove_from_node(parent, parent->key[1]);
            if (leaf->child[0] != NULL)
                child[1]->child[2] = leaf->child[0];
            else if (leaf->child[1] != NULL)
                child[1]->child[2] = leaf->child[1];

            if (child[1]->child[2] != NULL)
                child[1]->child[2]->parent = child[1];

            free(child[2]);
        }
    }
    else if ((parent->nkeys == 2) && ((child[0]->nkeys == 2) || (child[1]->nkeys == 2) || (child[2]->nkeys == 2)))
    {
        if (child[2] == leaf)
        {
            if (leaf->child[0] != NULL)
            {
                leaf->child[1] = leaf->child[0];
                leaf->child[0] = NULL;
            }

            insert_to_node(leaf, parent->key[1]);
            if (child[1]->nkeys == 2)
            {
                parent->key[1] = child[1]->key[1];
                remove_from_node(child[1], child[1]->key[1]);
                leaf->child[0] = child[1]->child[2];
                child[1]->child[2] = NULL;
                if (leaf->child[0] != NULL)
                    leaf->child[0]->parent = leaf;
            }
            else if (child[0]->nkeys == 2)
            {
                parent->key[1] = child[1]->key[0];
                leaf->child[0] = child[1]->child[1];
                child[1]->child[1] = child[1]->child[0];
                if (leaf->child[0] != NULL)
                    leaf->child[0]->parent = leaf;

                child[1]->key[0] = parent->key[0];
                parent->key[0] = child[0]->key[1];
                remove_from_node(child[0], child[0]->key[1]);
                child[1]->child[0] = child[0]->child[2];
                if (child[1]->child[0] != NULL)
                    child[1]->child[0]->parent = child[1];
                child[0]->child[2] = NULL;
            }
        }
        else if (child[1] == leaf)
        {
            if (child[2]->nkeys == 2)
            {
                if (leaf->child[0] == NULL)
                {
                    leaf->child[0] = leaf->child[1];
                    leaf->child[1] = NULL;
                }
                insert_to_node(child[1], parent->key[1]);
                parent->key[1] = child[2]->key[0];
                remove_from_node(child[2], child[2]->key[0]);
                child[1]->child[1] = child[2]->child[0];
                if (child[1]->child[1] != NULL)
                    child[1]->child[1]->parent = child[1];
                child[2]->child[0] = child[2]->child[1];
                child[2]->child[1] = child[2]->child[2];
                child[2]->child[2] = NULL;
            }
            else if (child[0]->nkeys == 2)
            {
                if (leaf->child[1] == NULL)
                {
                    leaf->child[1] = leaf->child[0];
                    leaf->child[0] = NULL;
                }
                insert_to_node(child[1], parent->key[0]);
                parent->key[0] = child[0]->key[1];
                remove_from_node(child[0], child[0]->key[1]);
                child[1]->child[0] = child[0]->child[2];
                if (child[1]->child[0] != NULL)
                    child[1]->child[0]->parent = child[1];
                child[0]->child[2] = NULL;
            }
        }
        else if (child[0] == leaf)
        {
            if (leaf->child[0] == NULL)
            {
                leaf->child[0] = leaf->child[1];
                leaf->child[1] = NULL;
            }
            insert_to_node(child[0], parent->key[0]);
            if (child[1]->nkeys == 2)
            {
                parent->key[0] = child[1]->key[0];
                remove_from_node(child[1], child[1]->key[0]);
                child[0]->child[1] = child[1]->child[0];
                if (child[0]->child[1] != NULL)
                    child[0]->child[1]->parent = child[0];
                child[1]->child[0] = child[1]->child[1];
                child[1]->child[1] = child[1]->child[2];
                child[1]->child[2] = NULL;
            }
            else if (child[2]->nkeys == 2)
            {
                parent->key[0] = child[1]->key[0];
                child[1]->key[0] = parent->key[1];
                parent->key[1] = child[2]->key[0];
                remove_from_node(child[2], child[2]->key[0]);
                child[0]->child[1] = child[1]->child[0];
                if (child[0]->child[1] != NULL)
                    child[0]->child[1]->parent = child[0];
                child[1]->child[0] = child[1]->child[1];
                child[1]->child[1] = child[2]->child[0];
                if (child[1]->child[1] != NULL)
                    child[1]->child[1]->parent = child[1];
                child[2]->child[0] = child[2]->child[1];
                child[2]->child[1] = child[2]->child[2];
                child[2]->child[2] = NULL;
            }
        }
    }
    else if (parent->nkeys == 1)
    {

        if (child[0] == leaf && child[1]->nkeys == 2)
        {
            parent->key[0] = child[1]->key[0];
            remove_from_node(child[1], child[1]->key[0]);

            if (leaf->child[0] == NULL)
                leaf->child[0] = leaf->child[1];

            leaf->child[1] = child[1]->child[0];
            child[1]->child[0] = child[1]->child[1];
            child[1]->child[1] = child[1]->child[2];
            child[1]->child[2] = NULL;
            if (leaf->child[1] != NULL)
                leaf->child[1]->parent = leaf;
        }
        else if (child[1] == leaf && child[0]->nkeys == 2)
        {
            parent->key[0] = child[0]->key[1];
            remove_from_node(child[0], child[0]->key[1]);

            if (leaf->child[1] == NULL)
                leaf->child[1] = leaf->child[0];

            leaf->child[0] = child[0]->child[2];
            child[0]->child[2] = NULL;
            if (leaf->child[0] != NULL)
                leaf->child[0]->parent = leaf;
        }
    }
    return parent;
}

btree *merge(btree *leaf)
{
    btree *parent = leaf->parent;
    if (parent->child[0] == leaf)
    {
        insert_to_node(parent->child[1], parent->key[0]);
        parent->child[1]->child[2] = parent->child[1]->child[1];
        parent->child[1]->child[1] = parent->child[1]->child[0];
        if (leaf->child[0] != NULL)
            parent->child[1]->child[0] = leaf->child[0];
        else if (leaf->child[1] != NULL)
            parent->child[1]->child[0] = leaf->child[1];
        if (parent->child[1]->child[0] != NULL)
            parent->child[1]->child[0]->parent = parent->child[1];
        remove_from_node(parent, parent->key[0]);
        free(parent->child[0]);
        parent->child[0] = NULL;
    }
    else if (parent->child[1] == leaf)
    {
        insert_to_node(parent->child[0], parent->key[0]);
        if (leaf->child[0] != NULL)
            parent->child[0]->child[2] = leaf->child[0];
        else if (leaf->child[1] != NULL)
            parent->child[0]->child[2] = leaf->child[1];
        if (parent->child[0]->child[2] != NULL)
            parent->child[0]->child[2]->parent = parent->child[0];
        remove_from_node(parent, parent->key[0]);
        free(parent->child[1]);
        parent->child[1] = NULL;
    }
    if (parent->parent == NULL)
    {
        btree *tmp = NULL;
        if (parent->child[0] != NULL)
            tmp = parent->child[0];
        else
            tmp = parent->child[1];
        tmp->parent = NULL;
        free(parent);
        return tmp;
    }
    return parent;
}

btree *fix(btree *leaf)
{
    if (leaf->nkeys == 0 && leaf->parent == NULL)
    {
        free(leaf);
        return NULL;
    }
    if (leaf->nkeys != 0)
    {
        if (leaf->parent)
            return fix(leaf->parent);
        else
            return leaf;
    }
    btree *parent = leaf->parent;
    if (parent->child[0]->nkeys == 2 || parent->child[1]->nkeys == 2 || parent->nkeys == 2)
        leaf = redistribute(leaf);
    else if (parent->nkeys == 2 && parent->child[2]->nkeys == 2)
        leaf = redistribute(leaf);
    else
        leaf = merge(leaf);
    return fix(leaf);
}

btree *del_key(btree *p, int k)
{
    btree *item = search(p, k);
    if (!item)
        return p;
    btree *min = NULL;
    if (item->key[0] == k)
        min = search_min(item->child[1]);
    else
        min = search_min(item->child[2]);
    if (min)
    {
        int *z = (k == item->key[0] ? &(item->key[0]) : &(item->key[1]));
        swap(z, &min->key[0]);
        item = min;
    }
    remove_from_node(item, k);
    return fix(item);
}