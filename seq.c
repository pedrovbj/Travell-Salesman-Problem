#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "circularArray.h"
#include "linkedList.h"

int getCost(int i, int j, int** g) {
    return (g[i][j]) ? g[i][j] : INT_MAX;
}

int pcv(int root, int current, circularArray* ca, int** g, linkedList* path) {
    int i;
    int cc;
    int cost = INT_MAX, costCandidate;
    circularArray* copies;
    linkedList* paths;
    int choosen = -1;

    if(ca->N == 1) {
        cost = getCost(current, ca->array[0], g) + getCost(ca->array[0], root, g);
        linkedListPush(root, path);
        linkedListPush(ca->array[0], path);
        linkedListPush(current, path);
        return cost;
    }

    copies = (circularArray*) malloc(ca->N*sizeof(circularArray));
    paths = (linkedList*) malloc(ca->N*sizeof(linkedList));
    for (i = 0; i < ca->N; i++) {
        circularArrayNew(ca->N-1, &copies[i]);
        linkedListNew(&paths[i]);
        cc = circularArrayReplicate(ca, &copies[i]);
        costCandidate = getCost(current, cc, g) + pcv(root, cc, &copies[i], g, &paths[i]);
        //linkedListPrint(&paths[i]);
        if (costCandidate < cost) {
            cost = costCandidate;
            choosen = i;
        }
    }

    linkedListCat(path, &paths[choosen]);
    linkedListPush(current, path);

    for (i = 0; i < ca->N; i++) {
        if (i != choosen) {
            linkedListDel(&paths[i]);
        }
    }

    for (i = 0; i < ca->N; i++) {
        circularArrayDel(&copies[i]);
    }
    free(copies);
    free(paths);
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

int main(int argc, char const *argv[]) {
    FILE*           fd;
    int**           adjMatrix;
    int             order;
    int             numProc;
    int             numThreads;
    int             i, j;
    circularArray   ca;
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

    //printf("%d, %d, %d\n", numProc, numThreads, order);

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

    for (i = 0; i < order; i++) {
        for (j = 0; j < order; j++) {
            printf("%d ", adjMatrix[i][j]);
        }
        printf("\n");
    }

    /* Cria e inicializa lista circular */
    circularArrayNew(order-1, &ca);
    circularArrayInit(&ca);

    /* Cria caminho */
    linkedListNew(&path);

    /* Imprime custo e caminho minimo */
    printf("%d\n", pcv(0, 0, &ca, adjMatrix, &path));
    linkedListPrint(&path);

    /* Desaloca a matriz de adjacencia */
    for (i = 0; i < order; i++) {
        free(adjMatrix[i]);
    }
    free(adjMatrix);
    circularArrayDel(&ca);
    exit(0);
}
