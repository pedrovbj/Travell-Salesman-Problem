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
#include <stdlib.h>

typedef struct {
    int*    array;
    int     index;
    int     N;
} circularArray;

/* Constructor */
void circularArrayNew(int N, circularArray* ca);

/* Destructor */
void circularArrayDel(circularArray* ca);

/* Inserts element 'a' at the position 'i' of ca's inner array */
void circularArrayInsert(int a, int i, circularArray* ca);

/* Shifts the index by +1 and returns the element at that position */
int circularArrayNext(circularArray* ca);

/* It replicates like a cell (looses a bit of it's telomere):
   The element at current index is lost at the copy,
   but it is returned by the function.
*/
int circularArrayReplicate(circularArray* ca, circularArray* copy);

/* Init circular array with values 1,..,N */
void circularArrayInit(circularArray* ca);
