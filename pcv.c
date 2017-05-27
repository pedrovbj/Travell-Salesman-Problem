#include <stdio.h>
#include <stdlib.h>
//#include "mpi.h"
#include <pthread.h>
#include <ctype.h>

int readNextInt(FILE* fd) {
    int c;

    c = fgetc(fd);
    while(c != EOF) {
        if (isdigit(c)) {
            /* converte ascii para digito */
            return c-'0';
        }
        c = fgetc(fd);
    }
    return -1;
}

int main(int argc, char const *argv[]) {
    FILE*   fd;
    int**   adjMatrix;
    int     order;
    int     numProc;
    int     numThreads;
    int     i, j;

    /* Verifica se a entrada tem o numero certo de argumentos */
    if (argc < 2) {
        printf("Usage: %s [file]\n", argv[0]);
        exit(-1);
    }

    /* Abre o arquivo de entrada */
    fd = fopen(argv[1], "r");
    if (fd == NULL) {
        printf("Error opening input file: \"%s\"\n", argv[1]);
        exit(-2);
    }

    /*
    Le em ordem:
    - Numero de processos
    - Numero de threads
    - Ordem da matriz
    */
    numProc = readNextInt(fd);
    numThreads = readNextInt(fd);
    order = readNextInt(fd);

    printf("%d, %d, %d\n", numProc, numThreads, order);

    /* Aloca a matriz de adjacencia */
    adjMatrix = (int**) malloc(order*sizeof(int**));
    for (i = 0; i < order; i++) {
        adjMatrix[i] = (int*) malloc(order*sizeof(int*));
    }

    /* Le matriz de adjacencia */
    for (i = 0; i < order; i++) {
        for (j = 0; j < order; j++) {
            adjMatrix[i][j] = readNextInt(fd);
        }
    }

    /* Fecha arquivo de entrada */
    fclose(fd);

    // for (i = 0; i < order; i++) {
    //     for (j = 0; j < order; j++) {
    //         printf("%d ", adjMatrix[i][j]);
    //     }
    //     printf("\n");
    // }

    /* Desaloca a matriz de adjacencia */
    for (i = 0; i < order; i++) {
        free(adjMatrix[i]);
    }
    free(adjMatrix);
    exit(0);
}
