#include "spmat.h"

spmat *spmat_init(index_t m, index_t n, index_t nz, index_t *ir, index_t *jc, num_t *vals)
{
    index_t k, p, nzcnt, *colcnts;
    spmat *A;

    A = malloc(sizeof(spmat));
    A->m = m;
    A->n = n;
    A->ir = malloc(nz * sizeof(index_t));
    A->jc = calloc(n+1, sizeof(index_t));
    A->vals = vals? malloc(nz * sizeof(num_t)) : NULL;
    colcnts = calloc(n+1, sizeof(index_t));

    for (k = 0; k < nz; ++k)
        colcnts[jc[k]]++;

    nzcnt = 0;
    for (k = 0; k < n; ++k)
    {
        A->jc[k] = nzcnt;
        nzcnt += colcnts[k];
        colcnts[k] = A->jc[k];
    }
    assert(nzcnt == nz);
    A->jc[n] = nzcnt;

    for (k = 0; k < nz; ++k)
    {
        p = colcnts[jc[k]]++;
        A->ir[p] = ir[k];
        if (vals) A->vals[p] = vals[k];
    }
    free(colcnts);
    return A;
}

void spmat_free(spmat *A)
{
    if (!A) return;
    A->m = A->n = 0;
    free(A->jc);
    free(A->ir);
    free(A->vals);
    free(A);
}
