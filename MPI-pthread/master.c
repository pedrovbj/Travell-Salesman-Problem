#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <pthread.h>
#include "mpi.h"
#include "linkedList.h"

int getCost(int i, int j, int** g) {
    return (g[i][j]) ? g[i][j] : INT_MAX;
}

int pcv(int root, int current, int** g, int order, linkedList* path,
        int numSlaves, int numThreads)
{
    int tag = 1;
    int myRank;
    int* array_of_errcodes;
    char slave[] = "./slave";
    MPI_Status status;
    MPI_Comm interComm;

    int i, j;
    int cost = INT_MAX;
    int* buf;
    int* pathArray;

    /* Um por slave */
    array_of_errcodes = (int*) malloc(numSlaves*sizeof(int));

    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    /* Spawn slaves */
    MPI_Comm_spawn(slave, MPI_ARGV_NULL, numSlaves, MPI_INFO_NULL, 0,
        MPI_COMM_WORLD, &interComm, array_of_errcodes);

    /* Send root */
    MPI_Send(&root, 1, MPI_INT, 0, tag, interComm);
    /* Send number of threads */
    MPI_Send(&numThreads, 1, MPI_INT, 0, tag++, interComm);
    /* Send order */
    MPI_Send(&order, 1, MPI_INT, 0, tag++, interComm);
    /* Envia matriz de adjacencia linha por linha, devido a alocacao dinamica */
    for (i = 0; i < order; i++) {
        MPI_Send(&g[i][0], order, MPI_INT, 0, tag++, interComm);
    }

    buf = (int*) malloc((order+1)*sizeof(int));
    pathArray = (int*) malloc(order*sizeof(int));

    /* Escuta promiscuamente pelos custos calculados nos slaves e atualiza
       o custo e o caminho minimo */
    for (i = 0; i < order-1; i++) {
        MPI_Recv(buf, order+1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
            interComm, &status);
        //printf("%d\n", buf[0]);
        if (buf[0] < cost) {
            cost = buf[0];
            for (j = 0; j < order; j++) {
                pathArray[j] = buf[j+1];
            }
        }
    }

    cost += getCost(root, pathArray[0], g);
    for (j = order-1; j >= 0; j--) {
        linkedListPush(pathArray[j], path);
    }
    linkedListPush(root, path);

    free(array_of_errcodes);
    free(buf);
    free(pathArray);
    return cost;
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
    printf("Caminho minimo: ");
    linkedListPrint(&path);
    printf("Custo minimo: %d\n", cost);

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
