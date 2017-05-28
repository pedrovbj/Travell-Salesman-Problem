#include "linkedList.h"

void linkedListNew(linkedList* l) {
    l->first = NULL;
    l->last = NULL;
}

void linkedListDel(linkedList* l) {
    while(!linkedListIsEmpty(l)) {
        linkedListPop(l);
    }
}

void linkedListPush(int a, linkedList* l) {
    node_t* node;

    node = (node_t*) malloc(sizeof(node));
    node->elem = a;
    node->next = l->first;
    l->first = node;
    if (l->last == NULL) {
        l->last = node;
    }
}

int linkedListPop(linkedList* l) {
    int a;
    node_t* node;

    if(linkedListIsEmpty(l)) {
        fprintf(stderr, "ERROR: Trying to pop from a empty linkedList\n");
        exit(-1);
    }

    node = l->first;
    l->first = node->next;
    a = node->elem;
    free(node);

    if(l->first == NULL) {
        l->last = NULL;
    }

    return a;
}

int linkedListIsEmpty(linkedList* l) {
    return (l->first == NULL);
}

void linkedListCat(linkedList* l1, linkedList* l2) {
    if (l1->last) {
        l1->last->next = l2->first;
    } else {
        l1->first = l2->first;
    }
    l1->last = l2->last;
    l2->first = NULL;
    l2->last = NULL;
}

void linkedListPrint(linkedList* l) {
    node_t* node;

    node = l->first;
    printf("[");
    while (node) {
        printf("%d", node->elem);
        if(node->next) {
            printf(", ");
        }
        node = node->next;
    }
    printf("]\n");
}
