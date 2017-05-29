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

int pcv(int root, int current, circularArray* ca, int** g, linkedList* path) {
    int i;
    int cc;
    int cost = INT_MAX, costCandidate;
    circularArray* copies;
    linkedList* paths;
    int choosen = -1;

    /* Caso base */
    if(ca->N == 1) {
        cost = getCost(current, ca->array[0], g) + getCost(ca->array[0], root, g);
        linkedListPush(root, path);
        linkedListPush(ca->array[0], path);
        linkedListPush(current, path);
        return cost;
    }

    /* Caso geral */
    copies = (circularArray*) malloc(ca->N*sizeof(circularArray));
    paths = (linkedList*) malloc(ca->N*sizeof(linkedList));
    for (i = 0; i < ca->N; i++) {
        linkedListNew(&paths[i]);
        cc = circularArrayReplicate(ca, &copies[i]);
        costCandidate = getCost(current, cc, g) + pcv(root, cc, &copies[i], g, &paths[i]);
        if (costCandidate < cost) {
            cost = costCandidate;
            choosen = i;
        }
    }

    /* Concatena o caminho minimo da sub-arvore com o caminho total */
    linkedListCat(path, &paths[choosen]);
    linkedListPush(current, path);

    /* Libera a memoria alocada */
    for (i = 0; i < ca->N; i++) {
        linkedListDel(&paths[i]);
    }
    free(paths);
    for (i = 0; i < ca->N; i++) {
        circularArrayDel(&copies[i]);
    }
    free(copies);
    return cost;
}

int main(int argc, char **argv) {
    int tag = 1;
    int myRank;
    int numProc;
    int nQuo, nRem;
    int beg, end;

    int current;
    int order;
    int root;
    int** g;
    int i, j;
    node_t* node;
    int costCandidate;
    int* buf;
    circularArray ca, caAux;
    linkedList path;

    MPI_Status status;
    MPI_Comm interComm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);

    MPI_Comm_get_parent(&interComm);

    if (myRank == 0) {
        MPI_Recv(&root, 1, MPI_INT, 0, tag, interComm, &status);
        MPI_Recv(&order, 1, MPI_INT, 0, tag++, interComm, &status);
        g = (int**) malloc(order*sizeof(int**));
        for (i = 0; i < order; i++) {
            g[i] = (int*) malloc(order*sizeof(int*));
        }
        for (i = 0; i < order; i++) {
            MPI_Recv(&g[i][0], order, MPI_INT, 0, tag++, interComm, &status);
        }
        // for (i = 0; i < order; i++) {
        //     for (j = 0; j < order; j++) {
        //         printf("%d ", g[i][j]);
        //     }
        //     printf("\n");
        // }
    }

    MPI_Bcast(&root, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&order, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // printf("%d, %d\n", order, myRank);

    if (myRank != 0) {
        g = (int**) malloc(order*sizeof(int**));
        for (i = 0; i < order; i++) {
            g[i] = (int*) malloc(order*sizeof(int*));
        }
    }

    for (i = 0; i < order; i++) {
        MPI_Bcast(&g[i][0], order, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // for (i = 0; i < order; i++) {
    //     for (j = 0; j < order; j++) {
    //         printf("%d ", g[i][j]);
    //     }
    //     printf("\n");
    // }

    /* Distribuicao de  carga */
    nQuo = (order-1) / numProc;
    nRem = (order-1) % numProc;

    if(myRank < nRem ) {
        beg = myRank*(nQuo+1);
        end = myRank*(nQuo+1)+nQuo;
    } else {
        beg = myRank*nQuo+nRem;
        end = (myRank+1)*nQuo+nRem-1;
    }

    //printf("<%d> %d, %d\n", myRank, beg, end);

    /* Cria e inicializa lista circular auxiliar */
    circularArrayNew(order-1, &caAux);
    circularArrayInit(&caAux);

    buf = (int*) malloc((order+1)*sizeof(int));

    /* pcv para cada sub-arvore atribuida ao processo */
    for (i = beg; i <= end; i++) {
        /* Cria caminho */
        linkedListNew(&path);

        /* Cria e inicializa lista circular */
        caAux.index = i;
        current = circularArrayReplicate(&caAux, &ca);
        // printf("<%d>curr=%d\n", myRank, current);
        // for (j = 0; j < order-2; j++)
        //     printf("<%d>%d; ", myRank, ca.array[j]);
        // puts("");

        costCandidate = pcv(root, current, &ca, g, &path);

        buf[0] = costCandidate;
        node = path.first;
        j = 1;
        while(node) {
            buf[j] = node->elem;
            node = node->next;
            j++;
        }
        // for (j = 0; j < order+1; j++)
        //      printf("<%d>%d,%d\n", myRank, buf[j], j);
        // puts("");
        MPI_Send(buf, order+1, MPI_INT, 0, tag, interComm);

        /* Destroi lista circular */
        circularArrayDel(&ca);
        /* Destroi o caminho */
        linkedListDel(&path);
    }

    /* Desaloca a matriz de adjacencia */
    for (i = 0; i < order; i++) {
        free(g[i]);
    }
    free(g);
    
    free(buf);
    circularArrayDel(&caAux);
    MPI_Finalize();
    exit(0);
}
