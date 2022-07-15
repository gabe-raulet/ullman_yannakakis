#ifndef UY_SPGRAPH_H_
#define UY_SPGRAPH_H_

#include <stdint.h> /* int64_t */
#include <stdlib.h> /* malloc, etc. */
#include <stdio.h> /* FILE, fscanf, etc. */
#include <assert.h>

typedef int64_t index_t;

typedef struct spgraph
{
    index_t n;     /* number of vertices */
    index_t *jc;   /* column pointers (size n+1) */
    index_t *ir;   /* row indices (size nz=jc[n]) */
} spgraph;

#define MAX(a, b) (((a) < (b))? (b) : (a))

spgraph *spgraph_init(index_t n, index_t nz, index_t *ir, index_t *jc);
spgraph *spgraph_load(FILE *f, int directed);
void spgraph_free(spgraph *g);

#endif
