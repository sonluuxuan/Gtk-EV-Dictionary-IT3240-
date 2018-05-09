#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bt-5.0.0/inc/btree.h"

void word_handle(char *word, int length)
{
    int i, j;
    for(i = 0; i < length; i ++) // remove phonetic //
    {
        if(word[i] == '/')
        {
            word[i] = '\0';
            break;
        }
    }
    for(j = 0; j < i; j ++) // remove '@' //
        word[j] = word[j + 1];
    // remove space at the end // abcde
    word[j - 2] = '\0';
}

void create_btree_dictionary(BTA *root)
{
    char line[200];
    char word[20];
    char translation[100000];
    translation[0] = '\0';
    FILE *fp;
    if((fp = fopen("anhviet.txt", "r")) == NULL)
    {
        printf("Can not open anhviet.txt!\n");
        return;
    }
    int i;
    int first_word_flag = 1;
    int word_count = 0;
    while((fgets(line, 200, fp)) != NULL)
    {
        if(line[0] == '@')
        {
            if(first_word_flag == 0)
            {
                btins(root, word, translation, strlen(translation));
                translation[0] = '\0';
            }
            word_handle(line, strlen(line));
            strcpy(word, line);
            first_word_flag = 0;
            word_count ++;
        }
        else
            strcat(translation, line);
    }
    printf("Inserted %d words into btree dictionary!\n", word_count);
    fclose(fp);
}

int main()
{
    BTA *tree;
    btinit();
    tree = btcrt("database", 0, 0);
    create_btree_dictionary(tree);
    btcls(tree);
    return 0;
}
