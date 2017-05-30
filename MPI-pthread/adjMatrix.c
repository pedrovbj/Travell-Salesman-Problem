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
#include "adjMatrix.h"
#include <stdio.h>

int getCost(int i, int j, int** g) {
    return g[i][j] ? g[i][j] : INT_MAX;
}

void updateCost(int *cost, int inc) {
    if ((inc == INT_MAX) || (*cost == INT_MAX)) {
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
