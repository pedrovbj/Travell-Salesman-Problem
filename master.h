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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mpi.h"
#include "linkedList.h"
#include "adjMatrix.h"

/*
 * Problema do Caixeiro Viajante
 *
 * Args de entrada:
 * root: cidade de origem
 * current: cidade atual
 * g: matriz de adjacencia
 * order: ordem da matriz de adj.
 * numSlaves: numero de processos escravos a serem criados
 * numThreads: numero de threads por escravo
 *
 * Args de saida:
 * path: melhor caminho
 *
 * Retorno: custo minimo
 */
int pcv(int root, int current, int** g, int order, linkedList* path,
        int numSlaves, int numThreads);

/* Le o proximo inteiro de fd (um inteiro por linha) */
int readNextInt(FILE* fd);

/* Funcao principal */
int main(int argc, char **argv);
