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

/* Inserts element a at the position i of ca's inner array */
void circularArrayInsert(int a, int i, circularArray* ca);

/* Shifts the index by +1 and returns the element at that position */
int circularArrayNext(circularArray* ca);

/* It replicates like DNA: the element at current index is lost
   at the copy, but is returned by this function.
*/
int circularArrayReplicate(circularArray* ca, circularArray* copy);

/* Init circular array with values 1,..,N */
void circularArrayInit(circularArray* ca);