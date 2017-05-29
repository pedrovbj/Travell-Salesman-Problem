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
#include "master.h"

int pcv(int root, int current, int** g, int order, linkedList* path,
        int numSlaves, int numThreads)
{
    int tag = 1;
    int myRank;
    int* array_of_errcodes;
    /* Tem que ser './slave' porque eh a localizacao do executavel
       que o MPI_Comm_spawn ira utilizar. Simplesmente 'slave' nao roda.*/
    char slave[] = "./slave";
    MPI_Status status;
    MPI_Comm interComm;
    int i, j;
    int cost = INT_MAX;
    int* buf; /* Buffer para receber candidatos a custo e caminho minimo */
    int* pathArray; /* Melhor caminho ate agora */

    /* Um codigo de erro por slave */
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

    /* Aloca buffer e pathArray */
    buf = (int*) malloc((order+1)*sizeof(int));
    pathArray = (int*) malloc(order*sizeof(int));

    /* Escuta promiscuamente pelos custos calculados nos slaves e atualiza
       o custo e o caminho minimo */
    for (i = 0; i < order-1; i++) {
        MPI_Recv(buf, order+1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
            interComm, &status);

        /* buf[0] contem o custo e o restante do buf o caminho */
        if (buf[0] < cost) {
            /* Atualiza custo minimo */
            cost = buf[0];
            /* Atualiza melhor caminho */
            for (j = 0; j < order; j++) {
                pathArray[j] = buf[j+1];
            }
        }
    }

    /* Acresce o custo de se ir do no raiz ate o primeiro no
       do caminho escolhido */
    updateCost(&cost, getCost(root, pathArray[0], g));

    /* Insere os elementos com o caminho escolhido na lista encadeada
       de retorno */
    for (j = order-1; j >= 0; j--) {
        linkedListPush(pathArray[j], path);
    }
    /* Insere raiz no caminho */
    linkedListPush(root, path);

    /* Clean up */
    free(array_of_errcodes);
    free(buf);
    free(pathArray);
    return cost;
}

int readNextInt(FILE* fd) {
    int aux;
    char buffer[20];

    fgets(buffer, sizeof(buffer), fd);
    sscanf(buffer, "%d%*s", &aux);

    return aux;
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
