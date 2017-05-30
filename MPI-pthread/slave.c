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
#include "slave.h"

/* Adj matrix */
int** g;
int order;

/* Just because the specification says the use of conditional variables is
   a __must__ */
int finished = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

int pcv_seq(int root, int current, circularArray* ca, linkedList* path) {
    int i, j;
    int cost = INT_MAX;
    int costCandidate;
    circularArray* cj;
    linkedList* pathj;
    int choosen = -1;

    /* Caso base */
    if(ca->N == 1) {
        linkedListPush(root, path);
        linkedListPush(ca->array[0], path);
        linkedListPush(current, path);
        cost = getCost(current, ca->array[0], g);
        updateCost(&cost, getCost(ca->array[0], root, g));
        return cost;
    }

    cj = (circularArray*) malloc(order*sizeof(circularArray));
    pathj = (linkedList*) malloc(order*sizeof(linkedList));
    /* Caso geral */
    for (i = 0; i < ca->N; i++) {
        j = ca->array[i];
        ca->index = i;
        circularArrayReplicate(ca, &cj[j]);
        linkedListNew(&pathj[j]);
        costCandidate = getCost(current, j, g);
        updateCost(&costCandidate, pcv_seq(root, j, &cj[j], &pathj[j]));
        if (costCandidate < cost) {
            cost = costCandidate;
            choosen = j;
        }
    }

    /* Concatena o caminho minimo da sub-arvore com o caminho total */
    linkedListCat(path, &pathj[choosen]);
    linkedListPush(current, path);

    /* Libera a memoria alocada */
    for (i = 0; i < ca->N; i++) {
        j = ca->array[i];
        if (j != choosen)
            linkedListDel(&pathj[j]);
    }
    free(pathj);
    for (i = 0; i < ca->N; i++) {
        j = ca->array[i];
        circularArrayDel(&cj[j]);
    }
    free(cj);

    return cost;
}

void* pcv_thread(void* tArgs) {
    threadArgs* args;
    task_t* task;
    task_t* aux;
    int costCandidate;
    linkedList pathAux;

    args = (threadArgs*) tArgs;

    /* Retira e realiza as tarefas das lista */
    task = args->tasklist;
    while(task) {
        linkedListNew(&pathAux);

        /* pcv sequencial */
        costCandidate = getCost(args->currentOfSlave, task->current, g);
        updateCost(&costCandidate, pcv_seq(args->root, task->current, task->ca, &pathAux));

        /* Atualiza o custo e qual o caminho escolhido */
        pthread_mutex_lock(args->lock);
        if(costCandidate < *args->cost) {
            *args->cost = costCandidate;
            *args->choosen = args->tid;
            linkedListDel((linkedList*)args->pathCandidate);
            linkedListNew((linkedList*)args->pathCandidate);
            linkedListCat((linkedList*)args->pathCandidate, &pathAux);
        } else {
            linkedListDel(&pathAux);
        }
        pthread_mutex_unlock(args->lock);

        /* Guarda proxima tarefa */
        aux = task->next;

        /* Clean up */
        circularArrayDel(task->ca);
        free(task->ca);
        free(task);

        /* Avanca pra proxima tarefa */
        task = aux;
    }

    /* Atualiza quantas threads terminaram e sinaliza a thread principal */
    pthread_mutex_lock(&mutex);
    finished++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(& mutex);

    pthread_exit(NULL);
}

int pcv(int root, int current, int order, circularArray* ca, linkedList* path,
        int numThreads)
{
    int nQuo, nRem;
    int beg, end;
    pthread_mutex_t lock;
    int cost = INT_MAX;
    int choosen = -1;
    int i, j, k, idx;
    task_t* task;
    pthread_t* thread;
    threadArgs* tArgs;

    /* Inicializacao dos mutex e variaveis de condicao */
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    /* Zero threads terminaram sua execucao */
    finished = 0;

    /* Cria array de threads */
    thread = (pthread_t*) malloc(numThreads*sizeof(pthread_t));

    /* Cria array de thread args */
    tArgs = (threadArgs*) malloc(numThreads*sizeof(threadArgs));

    /* Distribuicao de carga */
    nQuo = (order-2) / numThreads;
    nRem = (order-2) % numThreads;

    idx = 0;
    k = 0; /* Indice de ca */
    for(i = 0; i < numThreads; i++) {
        /* Atribui argumentos da thread */
        tArgs[i].tid = i;
        tArgs[i].cost = &cost;
        tArgs[i].choosen = &choosen;
        tArgs[i].root = root;
        tArgs[i].lock = &lock;
        tArgs[i].currentOfSlave = current;
        tArgs[i].pathCandidate = (void*) malloc(sizeof(linkedList));
        linkedListNew((linkedList*)tArgs[i].pathCandidate);

        /* Ponto inicial */
        beg = idx;

        /* Balanco de carga */
        if(i < nRem) {
            idx += nQuo+1;
        } else {
            idx += nQuo;
        }

        /* Ponto final */
        end = idx-1;

        /* Cria tasklist e atribui a thread */
        tArgs[i].tasklist = NULL;
        for(j = beg; j <= end; j++) {
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

    /* Join threads, the hard way */
    pthread_mutex_lock(&mutex);
    while(finished < numThreads) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    /* Atualiza melhor caminho */
    for (i = 0; i < numThreads; i++)
        if (i == choosen) {
        linkedListCat(path, tArgs[i].pathCandidate);
    } else {
        linkedListDel(tArgs[i].pathCandidate);
        free(tArgs[i].pathCandidate);
    }

    /* Inclui no atual no caminho */
    linkedListPush(current, path);

    /* Clean up */
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

    if (myRank == 0) {
        /* Recebe root, numero de threads e ordem da matriz de adj */
        MPI_Recv(&root, 1, MPI_INT, 0, tag, interComm, &status);
        MPI_Recv(&numThreads, 1, MPI_INT, 0, tag++, interComm, &status);
        MPI_Recv(&order, 1, MPI_INT, 0, tag++, interComm, &status);
        /* Aloca matriz de adj */
        allocateAdjMatrix(order, &g);
        /* Recebe linhas da matriz */
        for (i = 0; i < order; i++) {
            MPI_Recv(&g[i][0], order, MPI_INT, 0, tag++, interComm, &status);
        }
    }

    /* Broadcasting do root, numThreads e order */
    MPI_Bcast(&root, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&numThreads, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&order, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Outros processos alocam a matriz */
    if (myRank != 0) {
        allocateAdjMatrix(order, &g);
    }

    /* Broadcasting da matriz */
    for (i = 0; i < order; i++) {
        MPI_Bcast(&g[i][0], order, MPI_INT, 0, MPI_COMM_WORLD);
    }

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

    /* Cria e inicializa lista circular auxiliar */
    circularArrayNew(order-1, &caAux);
    circularArrayInit(&caAux);

    /* Aloca buffer para enviar candidatos a custo minimo e melhor caminho */
    buf = (int*) malloc((order+1)*sizeof(int));

    /* pcv para cada sub-arvore atribuida ao processo */
    for (i = beg; i <= end; i++) {
        /* Cria caminho */
        linkedListNew(&path);

        /* Cria e inicializa lista circular */
        caAux.index = i;
        current = circularArrayReplicate(&caAux, &ca);

        /* Calcula candidato a custo minimo */
        costCandidate = pcv(root, current, order, &ca, &path, numThreads);

        /* Preenche buffer de envio*/
        buf[0] = costCandidate;
        node = path.first;
        j = 1;
        while(node) {
            buf[j] = node->elem;
            node = node->next;
            j++;
        }

        /* Envia buffer ao master*/
        MPI_Send(buf, order+1, MPI_INT, 0, tag, interComm);

        /* Destroi lista circular */
        circularArrayDel(&ca);
        /* Destroi o caminho */
        linkedListDel(&path);
    }

    /* Clean up */
    freeAdjMatrix(order, g);
    free(buf);
    circularArrayDel(&caAux);

    /* Finaliza MPI */
    MPI_Finalize();
    exit(0);
}
