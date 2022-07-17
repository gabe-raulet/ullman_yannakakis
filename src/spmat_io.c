#include "spmat.h"
#include <inttypes.h>

spmat *spmat_load(FILE *f, int assert_square)
{
    index_t i, j, m, n, nz, nzmax, *ir, *jc;
    num_t v, *vals;
    spmat *A;
    char line[1024];

    if (!f) return NULL;

    while (fgets(line, 1024, f) && line[0] == '%');

    m = n = nz = 0;
    nzmax = 16;
    ir = malloc(nzmax * sizeof(index_t));
    jc = malloc(nzmax * sizeof(index_t));

    vals = (sscanf(line, "%" PRId64 " %" PRId64 " %lg", &i, &j, &v) == 2)? NULL : malloc(nzmax * sizeof(num_t));

    do
    {
        sscanf(line, "%" PRId64 " %" PRId64 " %lg", &i, &j, &v);
        ir[nz] = i-1;
        jc[nz] = j-1;
        if (vals) vals[nz] = v;
        m = MAX(m, i);
        n = MAX(n, j);

        if (++nz >= nzmax)
        {
            nzmax *= 2;
            ir = realloc(ir, nzmax * sizeof(index_t));
            jc = realloc(jc, nzmax * sizeof(index_t));
            if (vals) vals = realloc(vals, nzmax * sizeof(num_t));
        }
    } while (fgets(line, 1024, f));

    if (assert_square) m = n = MAX(m, n);

    A = spmat_init(m, n, nz, ir, jc, vals);
    free(ir);
    free(jc);
    free(vals);
    return A;
}

void spmat_write(spmat *A, FILE *f, int header)
{
    index_t i, j, p;

    if (header) fprintf(f, "%" PRId64 " %" PRId64 " %" PRId64 "\n", A->m, A->n, A->jc[A->n]);

    for (j = 0; j < A->n; ++j)
        for (p = A->jc[j]; p < A->jc[j+1]; ++p)
        {
            i = A->ir[p];
            if (A->vals) fprintf(f, "%" PRId64 " %" PRId64 " %lg\n", i+1, j+1, A->vals[p]);
            else fprintf(f, "%" PRId64 " %" PRId64 "\n", i+1, j+1);
        }
}
