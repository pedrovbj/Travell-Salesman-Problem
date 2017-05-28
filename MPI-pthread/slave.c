#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <pthread.h>
#include "mpi.h"
#include "circularArray.h"
#include "linkedList.h"

int main(int argc, char **argv) {
    int tag = 1;
    int myRank;
    int numProc;

    int order;
    int root;
    int** g;
    int i;
    struct {
        int cost;
        int id;
    } candidate;

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

    candidate.cost = pcv();
    candidate.id = myRank;
    MPI_Send(&candidate, 2, MPI_INT, 0, tag, interComm);

    MPI_Finalize();
    exit(0);
}
