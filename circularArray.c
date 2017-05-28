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

    circularArrayNew(ca->N-1, copy);

    aux = ca->array[ca->index];
    for (i = 0; i < ca->N-1; i++) {
        copy->array[i] = circularArrayNext(ca);
    }

    return aux;
}

void circularArrayInit(circularArray* ca) {
    int i;

    for (i = 0; i < ca->N; i++) {
        ca->array[i] = i+1;
    }
}
