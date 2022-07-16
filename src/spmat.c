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
    index_t i, j, m, n, nz, nzmax, *ir, *jc;
    spmat *A;

    if (!f) return NULL;

    m = n = nz = 0;
    nzmax = 16;
    ir = calloc(nzmax, sizeof(index_t));
    jc = calloc(nzmax, sizeof(index_t));

    char line[1024];

    while (fgets(line, 1024, f))
    {
        assert(sscanf(line, "%lld %lld", &i, &j)==2);
        ir[nz] = i;
        jc[nz++] = j;
        m = MAX(m, i+1);
        n = MAX(n, j+1);

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

void spmat_triples(const spmat *A, index_t **ir, index_t **jc)
{
    index_t j, p, nz, *_ir, *_jc;

    nz = A->jc[A->n];

    _ir = *ir = malloc(nz * sizeof(index_t));
    _jc = *jc = malloc(nz * sizeof(index_t));

    for (j = 0; j < A->n; ++j)
        for (p = A->jc[j]; p < A->jc[j+1]; ++p)
        {
            *_ir++ = A->ir[p];
            *_jc++ = j;
        }
}

spmat *spmat_transpose(const spmat *A)
{
    spmat *AT;
    index_t *ir, *jc;

    spmat_triples(A, &jc, &ir);
    AT = spmat_init(A->n, A->m, A->jc[A->n], ir, jc);
    free(ir);
    free(jc);
    return AT;
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
    index_t i, j, p, nadj, *rows;

    if (header) fprintf(f, "%lld %lld %lld\n", A->m, A->n, A->jc[A->n]);

    rows = NULL;

    for (j = 0; j < A->n; ++j)
    {
        nadj = A->jc[j+1] - A->jc[j];
        if (nadj > 0)
        {
            rows = realloc(rows, nadj * sizeof(index_t));
            memcpy(rows, A->ir + A->jc[j], nadj);
            qsort(rows, nadj, sizeof(index_t), index_compare);
            for (p = A->jc[j]; p < A->jc[j+1]; ++p)
            {
                i = A->ir[p];
                fprintf(f, "%lld %lld\n", i, j);
            }
        }
    }

    free(rows);
}
