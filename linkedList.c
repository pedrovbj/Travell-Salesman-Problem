/*
 ********************************************************
 * INSTITUTO DE CIENCIAS MATEMATICAS E DE COMPUTACAO DA *
 * UNIVERSIDADE DE SAO PAULO (ICMC-USP)                 *
 *                                                      *
 * Avaliacao Bimestral AB1 (26/05 a 29/05 de 2017)      *
 * Programacao Concorrente (SSC0742)                    *
 *                                                      *
 * Professor Paulo Sergio Lopes de Souza                *
 *                                                      *
 * Aluno Pedro Virgilio Basilio Jeronymo (8910559)      *
 *                                                      *
 ********************************************************
 * Problema do Caixeiro Viajante                        *
 ********************************************************
 */
#include "linkedList.h"

void linkedListNew(linkedList* l) {
    l->first = NULL;
    l->last = NULL;
    l->N = 0;
}

void linkedListDel(linkedList* l) {
    while(!linkedListIsEmpty(l)) {
        linkedListPop(l);
    }
}

void linkedListPush(int a, linkedList* l) {
    node_t* node;

    node = (node_t*) malloc(sizeof(node_t));
    node->elem = a;
    node->next = l->first;
    l->first = node;
    if (l->last == NULL) {
        l->last = node;
    }
    l->N++;
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
    l->N--;

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
    l1->N += l2->N;
    /* This empties l2 */
    linkedListNew(l2);
}

void linkedListPrint(linkedList* l) {
    node_t* node;

    node = l->first;
    while (node) {
        printf("%d", node->elem);
        if(node->next) {
            printf(", ");
        }
        node = node->next;
    }
    printf("\n");
}
