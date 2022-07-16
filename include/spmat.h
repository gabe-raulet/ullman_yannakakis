#ifndef UY_SPMAT_H_
#define UY_SPMAT_H_

#include <stdint.h> /* int64_t */
#include <stdlib.h> /* malloc, etc. */
#include <stdio.h> /* FILE, fscanf, etc. */
#include <assert.h>

typedef int64_t index_t;
typedef double num_t;

typedef struct spmat
{
    index_t m;   /* number of rows */
    index_t n;   /* number of columns */
    index_t *jc; /* column pointers (size n+1) */
    index_t *ir; /* row indices (size nz=jc[n]) */
    num_t *vals; /* nonzero array (size nz). NULL if matrix is binary */
} spmat;

#define MAX(a, b) (((a) < (b))? (b) : (a))

spmat *spmat_init(index_t m, index_t n, index_t nz, index_t *ir, index_t *jc, num_t *vals);
spmat *spmat_load(FILE *f);
void spmat_free(spmat *A);
void spmat_write(spmat *A, FILE *f, int header);
void spmat_triples(const spmat *A, index_t **ir, index_t **jc, num_t **vals);
spmat *spmat_transpose(const spmat *A);
void spmat_spmv_bool(const spmat *A, index_t *x, index_t *mask, int invert_mask);

#endif
