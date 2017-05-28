#include <stdlib.h>
#include <stdio.h>

typedef struct node_t {
    int elem;
    struct node_t* next;
} node_t;

typedef struct {
    node_t* first;
    node_t* last;
} linkedList;

void linkedListNew(linkedList* l);
void linkedListDel(linkedList* l);

void linkedListPush(int a, linkedList* l);
int linkedListPop(linkedList* l);
int linkedListIsEmpty(linkedList* l);

/* Destroys l2 */
void linkedListCat(linkedList* l1, linkedList* l2);

void linkedListPrint(linkedList* l);
