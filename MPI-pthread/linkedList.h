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
#include <stdlib.h>
#include <stdio.h>

typedef struct node_t {
    int elem;
    struct node_t* next;
} node_t;

typedef struct {
    node_t* first;
    node_t* last;
    int N;
} linkedList;

/* Constructor */
void linkedListNew(linkedList* l);

/* Destructor */
void linkedListDel(linkedList* l);

/* Pushes the element 'a' into the list's head */
void linkedListPush(int a, linkedList* l);

/* Pops out the first element in the list */
int linkedListPop(linkedList* l);

/* Returns True if the list is empty and False otherwise */
int linkedListIsEmpty(linkedList* l);

/* Concatenates the tail of l1 with the head of l2, but __empties l2__ */
void linkedListCat(linkedList* l1, linkedList* l2);

/* Prints list to stdout */
void linkedListPrint(linkedList* l);
