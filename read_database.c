#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bt-5.0.0/inc/btree.h"

int main()
{
    BTA *tree;
    btinit();
    tree = btopn("database", 1, 0);
    char word[20];
    char translation[100000];
    int real_size;
    printf("Enter your word: ");
    scanf("%s", word);
    btsel(tree, word, translation, 100000, &real_size);
    printf("%s", translation);
    btcls(tree);
    return 0;
}
