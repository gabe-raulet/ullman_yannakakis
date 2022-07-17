#include "spmat.h"
#include <string.h> /* memcpy */

void spmat_spmv_bool(const spmat *A, index_t *x, index_t *mask, int invert_mask)
{
    index_t n, i, j, p, *w;

    n = A->n;
    w = calloc(n, sizeof(index_t));

    for (j = 0; j < n; ++j)
        if (x[j])
            for (p = A->jc[j]; p < A->jc[j+1]; ++p)
            {
                i = A->ir[p];
                if ((!!mask[i]) != (!!invert_mask)) w[i] = 1;
            }

    memcpy(x, w, sizeof(index_t) * n);
    free(w);
}
