#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mpi.h"
#include "circularArray.h"
#include "linkedList.h"
#include "adjMatrix.h"

/* Contem as informacoes para uma tarefa que a thread tem de realizar
 * Essas informacoes sao (current, ca), o que permite a thread calcular
 * pcv_seq(current, {ca array}).
 *
 * Tambem aponta para a proxima tarefa a ser realizada.
*/
typedef struct task_t {
    int current;
    circularArray* ca;
    struct task_t* next;
} task_t;

/* Argumentos da thread */
typedef struct {
    int tid;
    int root;
    int* cost; /* Variavel compartilhada */
    int* choosen; /* Variavel compartilhada */
    pthread_mutex_t* lock;
    task_t* tasklist;
    void* pathCandidate;
} threadArgs;

/* Problema do caxeiro viajante sequencial */
int pcv_seq(int root, int current, circularArray* ca, linkedList* path);

/* Main das threads */
void* pcv_thread(void* tArgs);

/* Invoca threads que resolver o pcv sequencialmente para a sua sub-arvore */
int pcv(int root, int current, int order, circularArray* ca, linkedList* path,
        int numThreads);

/* Funcao principal */
int main(int argc, char **argv);
