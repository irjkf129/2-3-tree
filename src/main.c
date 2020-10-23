#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

int main()
{
    node *root = NULL;
    root = insert(root, 1);
    root = insert(root, 2);
    root = insert(root, 3);
    root = insert(root, 4);
    root = insert(root, 5);
    root = insert(root, 6);
    root = insert(root, 7);
    //printf("%d\n", search(root, 7)->key[0]);
    return 0;
}