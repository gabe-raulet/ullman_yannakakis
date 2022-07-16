#include "spmat.h"

spmat *spmat_init(index_t m, index_t n, index_t nz, index_t *ir, index_t *jc)
{
    index_t k, p, nzcnt, *colcnts;
    spmat *A;

    A = malloc(sizeof(spmat));
    A->m = m;
    A->n = n;
    A->ir = malloc(nz * sizeof(index_t));
    A->jc = calloc(n+1, sizeof(index_t));
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
    free(A);
}

spmat *spmat_load(FILE *f)
{
    index_t u, v, m, n, nz, nzmax, *ir, *jc;
    spmat *A;

    if (!f) return NULL;

    m = n = nz = 0;
    nzmax = 16;
    ir = calloc(nzmax, sizeof(index_t));
    jc = calloc(nzmax, sizeof(index_t));

    char line[1024];

    while (fgets(line, 1024, f))
    {
        assert(sscanf(line, "%lld %lld", &u, &v)==2);
        ir[nz] = u-1;
        jc[nz++] = v-1;
        m = MAX(m, u);
        n = MAX(n, v);

        if (nz >= nzmax)
        {
            nzmax *= 2;
            ir = realloc(ir, nzmax * sizeof(index_t));
            jc = realloc(jc, nzmax * sizeof(index_t));
        }
    }

    A = spmat_init(m, n, nz, ir, jc);
    free(ir);
    free(jc);
    return A;
}

int index_compare(const void *_i1, const void *_i2)
{
    index_t i1, i2;
    i1 = *((index_t*)_i1);
    i2 = *((index_t*)_i2);

    if (i1 < i2) return -1;
    else if (i1 > i2) return 1;
    else return 0;
}

#include <string.h>

void spmat_write(spmat *A, FILE *f, int header)
{
    index_t v, p, nadj, *rows;

    if (header) fprintf(f, "%lld %lld %lld\n", A->m, A->n, A->jc[A->n]);

    rows = NULL;

    for (v = 0; v < A->n; ++v)
    {
        nadj = A->jc[v+1] - A->jc[v];
        if (nadj > 0)
        {
            rows = realloc(rows, nadj * sizeof(index_t));
            memcpy(rows, A->ir + A->jc[v], nadj);
            qsort(rows, nadj, sizeof(index_t), index_compare);
            for (p = A->jc[v]; p < A->jc[v+1]; ++p)
            {
                index_t u = A->ir[p];
                fprintf(f, "%lld %lld\n", u+1, v+1);
            }
        }
    }

    free(rows);
}
