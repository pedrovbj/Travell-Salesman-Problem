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
#include <limits.h>

/* Retorna o custo entre os vertices i e j de g. Se esta aresta nao existir,
   o custo retornado eh INT_MAX */
int getCost(int i, int j, int** g);

/* Soma o incremento inc ao valor de cost.
   Caso inc == INT_MAX, cost recebe INT_MAX */
void updateCost(int *cost, int inc);

/* Aloca matriz de adjacencia */
void allocateAdjMatrix(int order, int*** g);

/* Desaloca matriz de adjacencia */
void freeAdjMatrix(int order, int** g);
