#include "spmat.h"

spmat *spmat_transpose(const spmat *A)
{
    spmat *AT;
    index_t *ir, *jc;
    num_t *vals;

    vals = NULL;
    spmat_triples(A, &jc, &ir, (A->vals)? &vals : NULL);
    AT = spmat_init(A->n, A->m, A->jc[A->n], ir, jc, vals);
    free(ir);
    free(jc);
    free(vals);
    return AT;
}
