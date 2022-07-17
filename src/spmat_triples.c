#include "spmat.h"

/* This function has undefined behaviour when: (vals != NULL) && A->vals == NULL) */
void spmat_triples(const spmat *A, index_t **ir, index_t **jc, num_t **vals)
{
    index_t j, p, nz, *_ir, *_jc;
    num_t *_vals;

    nz = A->jc[A->n];
    _vals = NULL;

    _ir = *ir = malloc(nz * sizeof(index_t));
    _jc = *jc = malloc(nz * sizeof(index_t));
    if (vals) _vals = *vals = malloc(nz * sizeof(num_t));

    for (j = 0; j < A->n; ++j)
        for (p = A->jc[j]; p < A->jc[j+1]; ++p)
        {
            *_ir++ = A->ir[p];
            *_jc++ = j;
            if (vals) *_vals++ = A->vals[p];
        }
}

