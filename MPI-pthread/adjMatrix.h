#include <stdlib.h>
#include <limits.h>

/* Retorna o custo entre os vertices i e j de g. Se esta aresta nao existir,
   o custo retornado eh INT_MAX */
int getCost(int i, int j, int** g);

/* Soma o incremento inc ao valor de cost.
   Caso inc == INT_MAX, cost recebe INT_MAX */
void updateCost(int *cost, int inc);

/* Aloca matriz de adjacencia */
void allocateAdjMatrix(int order, int*** g);

/* Desaloca matriz de adjacencia */
void freeAdjMatrix(int order, int** g);
