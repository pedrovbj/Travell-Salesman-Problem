#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <pthread.h>
#include "mpi.h"
#include "circularArray.h"
#include "linkedList.h"

int getCost(int i, int j, int** g) {
    return (g[i][j]) ? g[i][j] : INT_MAX;
}

int pcv(int root, int current, int** g, int order, linkedList* path,
        int numSlaves, int numThreads)
{
    int tag = 1;
    int cost;
    int myRank;
    int* array_of_errcodes;
    char slave[] = "./slave";
    MPI_Status status;
    MPI_Comm interComm;
    circularArray ca;
    int i;

    /* Cria e inicializa lista circular */
    circularArrayNew(order-1, &ca);
    circularArrayInit(&ca);

    /* Um por slave */
    array_of_errcodes = (int*) malloc(numSlaves*sizeof(int));

    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    MPI_Comm_spawn(slave, MPI_ARGV_NULL, numSlaves, MPI_INFO_NULL, 0,
        MPI_COMM_WORLD, &interComm, array_of_errcodes);

    MPI_Send(&order, 1, MPI_INT, 0, tag, interComm);
    for (i = 0; i < order; i++) {
        MPI_Send(&g[i][0], order, MPI_INT, 0, tag++, interComm);
    }

    circularArrayDel(&ca);
    free(array_of_errcodes);
    return 0;
}

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

int main(int argc, char **argv) {
    FILE*           fd;
    int**           adjMatrix;
    int             order;
    int             numProc;
    int             numThreads;
    int             i, j;
    int             cost;
    linkedList      path;

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

    /* Inicializa o MPI*/
    MPI_Init(&argc, &argv);

    // for (i = 0; i < order; i++) {
    //     for (j = 0; j < order; j++) {
    //         printf("%d ", adjMatrix[i][j]);
    //     }
    //     printf("\n");
    // }


    /* Cria caminho */
    linkedListNew(&path);

    /* Calcula custo e caminho minimo */
    cost = pcv(0, 0, adjMatrix, order, &path, numProc-1, numThreads);

    /* Imprime custo e caminho minimo */
    //printf("Caminho minimo: ");
    //linkedListPrint(&path);
    //printf("Custo minimo: %d\n", cost);

    /* Desaloca a matriz de adjacencia */
    for (i = 0; i < order; i++) {
        free(adjMatrix[i]);
    }
    free(adjMatrix);

    /* Desaloca caminho */
    linkedListDel(&path);

    /* Finaliza o MPI*/
    MPI_Finalize();

    exit(0);
}
