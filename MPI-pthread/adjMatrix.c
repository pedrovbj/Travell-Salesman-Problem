#include "adjMatrix.h"
#include <stdio.h>

int getCost(int i, int j, int** g) {
    return g[i][j] ? g[i][j] : INT_MAX;
}

void updateCost(int *cost, int inc) {
    if (inc == INT_MAX) {
        *cost = INT_MAX;
    } else {
        (*cost) += inc;
    }
}

void allocateAdjMatrix(int order, int*** g) {
    int i;
    *g = (int**) malloc(order*sizeof(int*));
    for (i = 0; i < order; i++) {
        (*g)[i] = (int*) malloc(order*sizeof(int));
    }
}

void freeAdjMatrix(int order, int** g) {
    int i;
    for (i = 0; i < order; i++) {
        free(g[i]);
    }
    free(g);
}
