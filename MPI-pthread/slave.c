#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <pthread.h>
#include "mpi.h"
#include "circularArray.h"
#include "linkedList.h"

typedef struct task_t {
    int current;
    circularArray* ca;
    struct task_t* next;
} task_t;

typedef struct {
    int tid;
    int root;
    int* cost;
    int* choosen;
    pthread_mutex_t* lock;
    task_t* tasklist;
} threadArgs;

/* Adj matrix */
int** g;

int getCost(int i, int j) {
    return g[i][j] ? g[i][j] : INT_MAX;
}

int pcv_seq(int root, int current, circularArray* ca, linkedList* path) {
    int i;
    int cc;
    int cost = INT_MAX, costCandidate;
    circularArray* copies;
    linkedList* paths;
    int choosen = -1;

    /* Caso base */
    if(ca->N == 1) {
        cost = getCost(current, ca->array[0]) + getCost(ca->array[0], root);
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
        costCandidate = getCost(current, cc) + pcv_seq(root, cc, &copies[i], &paths[i]);
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

void* pcv_thread(void* tArgs) {
    threadArgs* args;
    task_t* task;
    task_t* aux;
    int costCandidate;
    void* pathCandidate;
    linkedList pathAux;

    args = (threadArgs*) tArgs;

    pathCandidate = (void*) malloc(sizeof(linkedList));
    linkedListNew((linkedList*)pathCandidate);

    task = args->tasklist;
    while(task) {
        linkedListNew(&pathAux);
        //seq int root, int current, circularArray* ca, linkedList* path
        costCandidate = pcv_seq(args->root, task->current, task->ca, &pathAux);
        //printf("<<%d, %d> %d>\n", args->tid, getpid(), costCandidate);

        pthread_mutex_lock(args->lock);
        //printf("<<%d, %d> %d LOCKED>\n", args->tid, getpid(), costCandidate);
        if(costCandidate < *args->cost) {
            //printf("%d\n", *args->cost);
            *args->cost = costCandidate;
            *args->choosen = args->tid;
            linkedListDel((linkedList*)pathCandidate);
            linkedListNew((linkedList*)pathCandidate);
            linkedListCat((linkedList*)pathCandidate, &pathAux);
            //linkedListPrint((linkedList*)pathCandidate);
        } else {
            linkedListDel(&pathAux);
        }
        //printf("<<%d, %d> %d UNLOCKED>\n", args->tid, getpid(), costCandidate);
        pthread_mutex_unlock(args->lock);

        // printf("<%d> %d, {", args->tid, task->current);
        // int i;
        // for (i = 0; i < task->ca->N; i++) {
        //     printf("%d", task->ca->array[i]);
        // }
        // printf("}\n");
        aux = task->next;
        circularArrayDel(task->ca);
        free(task->ca);
        free(task);
        task = aux;
    }

    pthread_exit(pathCandidate);
}

int pcv(int root, int current, int order, circularArray* ca, linkedList* path,
        int numThreads)
{
    int nQuo, nRem;
    int beg, end;
    pthread_mutex_t lock;
    int cost = INT_MAX;
    int choosen = -1;
    void* choosenPath;
    int i, j, k, idx;
    task_t* task;
    pthread_t* thread;
    threadArgs* tArgs;

    // int a = 1;
    // printf("<PID %d ready to attach>\n", getpid());
    // while(a){
    // }

    //pthread_init();
    pthread_mutex_init(&lock, NULL);

    //Create array of threads
    thread = (pthread_t*) malloc(numThreads*sizeof(pthread_t));

    //Create array of thread args
    tArgs = (threadArgs*) malloc(numThreads*sizeof(threadArgs));

    /* Distribuicao de carga */
    nQuo = (order-2) / numThreads;
    nRem = (order-2) % numThreads;

    idx = 0;
    k = 0;
    for(i = 0; i < numThreads; i++) {
        tArgs[i].tid = i;
        tArgs[i].cost = &cost;
        tArgs[i].choosen = &choosen;
        tArgs[i].root = root;
        tArgs[i].lock = &lock;
        //Sets start point
        beg = idx;

        //Load balancing
        if(i < nRem) {
            idx += nQuo+1;
        } else {
            idx += nQuo;
        }

        //Sets end point
        end = idx-1;

        tArgs[i].tasklist = NULL;
        for(j = beg; j <= end; j++) {
            //printf("<%d><%d, %d>\n", getpid(), beg, end);
            task = (task_t*) malloc(sizeof(task_t));

            task->ca = (circularArray*) malloc(sizeof(circularArray));
            circularArrayNew(ca->N-1, task->ca);

            ca->index = k;
            task->current = circularArrayReplicate(ca, task->ca);
            k++;

            task->next = tArgs[i].tasklist;
            tArgs[i].tasklist = task;
        }

        /* Criacao das threads */
        pthread_create(&thread[i], NULL, pcv_thread, (void*) &tArgs[i]);
    }

    /* Join threads */
    for (i = 0; i < numThreads; i++) {
        pthread_join(thread[i], &choosenPath);
        //linkedListPrint(choosenPath);
        if (choosen == i) {
            //printf("<PID %d> choosen = %d\n", getpid(), choosen);
            linkedListCat(path, choosenPath);
            //linkedListPrint(path);
        } else {
            linkedListDel(choosenPath);
            free(choosenPath);
        }
    }

    //printf("<PID %d, cost = %d, current=%d, path->first->elem= %d>\n", getpid(), cost,current, path->first->elem);
    cost += getCost(current, path->first->elem);
    //printf("<PID %d, cost after = %d>\n", getpid(), cost);
    linkedListPush(current, path);

    free(thread);
    free(tArgs);
    return cost;
}

int main(int argc, char **argv) {
    int tag = 1;
    int myRank;
    int numProc;
    int nQuo, nRem;
    int beg, end;
    int numThreads;

    int current;
    int order;
    int root;
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

    printf("<Slave (%d) PID %d>\n", myRank, getpid());

    if (myRank == 0) {
        MPI_Recv(&root, 1, MPI_INT, 0, tag, interComm, &status);
        MPI_Recv(&numThreads, 1, MPI_INT, 0, tag++, interComm, &status);
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
    MPI_Bcast(&numThreads, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&order, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //printf("<%d, %d, %d>\n", order, numThreads, myRank);

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

    /* Distribuicao de carga */
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

        costCandidate = pcv(root, current, order, &ca, &path, numThreads);

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
