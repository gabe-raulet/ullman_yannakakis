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

spmat *spmat_load(FILE *f)
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

    vals = (sscanf(line, "%lld %lld %lg", &i, &j, &v) == 2)? NULL : malloc(nzmax * sizeof(num_t));

    do
    {
        sscanf(line, "%lld %lld %lg", &i, &j, &v);
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

    A = spmat_init(m, n, nz, ir, jc, vals);
    free(ir);
    free(jc);
    free(vals);
    return A;
}

/* This function has undefined behaviour when: (vals != NULL) && A->vals == NULL) */
void spmat_triples(const spmat *A, index_t **ir, index_t **jc, num_t **vals)
{
    index_t j, p, nz, *_ir, *_jc;
    num_t *_vals;

    nz = A->jc[A->n];

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

void spmat_write(spmat *A, FILE *f, int header)
{
    index_t i, j, p;

    if (header) fprintf(f, "%lld %lld %lld\n", A->m, A->n, A->jc[A->n]);

    for (j = 0; j < A->n; ++j)
        for (p = A->jc[j]; p < A->jc[j+1]; ++p)
        {
            i = A->ir[p];
            if (A->vals) fprintf(f, "%lld %lld %lg\n", i+1, j+1, A->vals[p]);
            else fprintf(f, "%lld %lld\n", i+1, j+1);
        }
}
