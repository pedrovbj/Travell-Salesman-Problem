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
#include "circularArray.h"

void circularArrayNew(int N, circularArray* ca) {
    ca->array = (int*) malloc(N*sizeof(int));
    ca->index = 0;
    ca->N = N;
}

void circularArrayDel(circularArray* ca) {
    if(ca->array)
        free(ca->array);
    ca->array = NULL;
    ca->index = 0;
}

void circularArrayInsert(int a, int i, circularArray* ca) {
    ca->array[i] = a;
}

int circularArrayNext(circularArray* ca) {
    ca->index = (ca->index+1) % ca->N;
    return ca->array[ca->index];
}

int circularArrayReplicate(circularArray* ca, circularArray* copy) {
    int i;
    int aux;

    /* One element shorter, because cells that replicate forever
       are called cancer */
    circularArrayNew(ca->N-1, copy);

    /* aux <- element at current index */
    aux = ca->array[ca->index];
    /* Because circularArrayNext increments the index before returning
       an element, the value stored at aux is not copied */
    for (i = 0; i < ca->N-1; i++) {
        copy->array[i] = circularArrayNext(ca);
    }

    /* Returns that missing element in the copy */
    return aux;
}

void circularArrayInit(circularArray* ca) {
    int i;

    for (i = 0; i < ca->N; i++) {
        ca->array[i] = i+1;
    }
}
