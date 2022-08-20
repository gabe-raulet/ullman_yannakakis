#include "spmat.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

spmat* spmat_init(long nrows, long ncols, long nnz, const long *rowidxs, const long *colidxs)
{
    spmat *mat;
    long *ir, *jc, *colcnts, k, nzcnt;

    ir = malloc(nnz * sizeof(long));
    jc = calloc(ncols+1, sizeof(long));
    colcnts = calloc(ncols+1, sizeof(long));

    for (k = 0; k < nnz; ++k)
        ++colcnts[colidxs[k]];

    nzcnt = 0;

    for (k = 0; k < ncols; ++k)
    {
        jc[k] = nzcnt;
        nzcnt += colcnts[k];
        colcnts[k] = jc[k];
    }

    jc[ncols] = nzcnt;

    for (k = 0; k < nnz; ++k)
        ir[colcnts[colidxs[k]]++] = rowidxs[k];

    free(colcnts);

    mat = malloc(sizeof(spmat));
    mat->m = nrows;
    mat->n = ncols;
    mat->jc = jc;
    mat->ir = ir;

    return mat;
}

spmat* spmat_read(FILE *f)
{
    char line[1024];
    long m, n, nz, *rowidxs, *colidxs, k, i, j;
    spmat *mat;

    assert(fgets(line, 1024, f)!=NULL);
    assert(sscanf(line, "%ld %ld %ld", &m, &n, &nz)==3);

    rowidxs = malloc(nz * sizeof(long));
    colidxs = malloc(nz * sizeof(long));

    for (k = 0; k < nz; ++k)
    {
        assert(fgets(line, 1024, f)!=NULL);
        assert(sscanf(line, "%ld %ld", &i, &j)==2);

        rowidxs[k] = i-1;
        colidxs[k] = j-1;
    }

    mat = spmat_init(m, n, nz, rowidxs, colidxs);

    free(rowidxs);
    free(colidxs);

    return mat;
}

spmat* spmat_copy(const spmat *mat)
{
    spmat *matcopy;

    matcopy = malloc(sizeof(spmat));
    matcopy->m = getnrows(mat);
    matcopy->n = getncols(mat);
    matcopy->jc = malloc((getncols(mat)+1) * sizeof(long));
    matcopy->ir = malloc(getnnz(mat) * sizeof(long));
    memcpy(matcopy->jc, mat->jc, (getncols(mat)+1) * sizeof(long));
    memcpy(matcopy->ir, mat->ir, getnnz(mat) * sizeof(long));

    return matcopy;
}

void spmat_free(spmat *mat)
{
    mat->m = mat->n = 0;
    free(mat->jc);
    free(mat->ir);
    free(mat);
}

spmat* spmat_transpose(const spmat *mat)
{
    spmat *transpose;
    long m, n, nz, j, k, p, *rowidxs, *colidxs;

    m = getnrows(mat);
    n = getncols(mat);
    nz = getnnz(mat);

    rowidxs = malloc(nz * sizeof(long));
    colidxs = malloc(nz * sizeof(long));

    k = 0;

    for (j = 0; j < n; ++j)
        for (p = mat->jc[j]; p < mat->jc[j+1]; ++p)
        {
            rowidxs[k] = j;
            colidxs[k] = mat->ir[p];
            ++k;
        }

    transpose = spmat_init(n, m, nz, rowidxs, colidxs);

    free(rowidxs);
    free(colidxs);

    return transpose;
}

spmat* spmat_spgemm(const spmat *A, const spmat *B)
{

    spmat *C;
    bitmap *spa;
    long m, n, nz, nzmax, *jc, *ir, j, kp, k, ip, i;

    if (getncols(A) != getnrows(B))
        return NULL;

    m = getnrows(A);
    n = getncols(B);
    nzmax = getnnz(A) + getnnz(B);
    nz = 0;

    jc = malloc((n+1) * sizeof(long));
    ir = malloc(nzmax * sizeof(long));

    spa = bitmap_init(m);

    for (j = 0; j < n; ++j)
    {
        jc[j] = nz;

        if (nz + m > nzmax)
        {
            nzmax = nz + m;
            ir = realloc(ir, nzmax * sizeof(long));
        }

        bitmap_reset(spa);

        for (kp = B->jc[j]; kp < B->jc[j+1]; ++kp)
        {
            k = B->ir[kp];

            for (ip = A->jc[k]; ip < A->jc[k+1]; ++ip)
            {
                i = A->ir[ip];

                if (!bitmap_get(spa, i))
                {
                    ir[nz++] = i;
                    bitmap_set(spa, i);
                }
            }
        }
    }

    jc[n] = nz;

    bitmap_free(spa);

    ir = realloc(ir, nz * sizeof(long));

    C = malloc(sizeof(spmat));
    C->m = m;
    C->n = n;
    C->jc = jc;
    C->ir = ir;

    return C;
}

spmat* spmat_add(const spmat *A, const spmat *B)
{
    spmat *C;
    bitmap *spa;
    long m, n, nz, nzmax, *jc, *ir, j, ip;

    m = getnrows(A);
    n = getncols(B);

    if (m != getnrows(B) || n != getncols(B))
        return NULL;

    nz = 0;
    nzmax = getnnz(A) + getnnz(B);

    jc = malloc((n+1) * sizeof(long));
    ir = malloc(nzmax * sizeof(long));

    spa = bitmap_init(m);

    for (j = 0; j < n; ++j)
    {
        bitmap_reset(spa);

        jc[j] = nz;

        for (ip = A->jc[j]; ip < A->jc[j+1]; ++ip)
            bitmap_set(spa, A->ir[ip]);

        for (ip = B->jc[j]; ip < B->jc[j+1]; ++ip)
            bitmap_set(spa, B->ir[ip]);

        nz += bitmap_indices(spa, ir + nz);
    }

    jc[n] = nz;

    bitmap_free(spa);

    ir = realloc(ir, nz * sizeof(long));

    C = malloc(sizeof(spmat));
    C->m = m;
    C->n = n;
    C->jc = jc;
    C->ir = ir;

    return C;
}

spmat* spmat_diff(const spmat *A, const spmat *B)
{
    spmat *C;
    bitmap *b;
    long m, n, nz, nzmax, *jc, *ir, j, p;

    m = getnrows(A);
    n = getncols(B);

    if (m != getnrows(B) || n != getncols(B))
        return NULL;

    nz = 0;
    nzmax = getnnz(A);

    jc = malloc((n+1) * sizeof(long));
    ir = malloc(nzmax * sizeof(long));

    b = bitmap_init(m);

    for (j = 0; j < n; ++j)
    {
        jc[j] = nz;

        for (p = B->jc[j]; p < B->jc[j+1]; ++p)
            bitmap_set(b, B->ir[p]);

        for (p = A->jc[j]; p < A->jc[j+1]; ++p)
            if (!bitmap_get(b, A->ir[p]))
                ir[nz++] = A->ir[p];

        bitmap_reset(b);
    }

    jc[n] = nz;

    bitmap_free(b);

    ir = realloc(ir, nz * sizeof(long));

    C = malloc(sizeof(spmat));
    C->m = m;
    C->n = n;
    C->jc = jc;
    C->ir = ir;

    return C;
}

spmat* spmat_init_msbfs(const spmat *graph, const long *sources, long num_sources)
{
    spmat *mat;
    long *jc, *ir, i;

    if (getnrows(graph) != getncols(graph))
        return NULL;

    jc = malloc((num_sources+1) * sizeof(long));
    ir = malloc(num_sources * sizeof(long));

    for (i = 0; i < num_sources; ++i)
    {
        jc[i] = i;
        ir[i] = sources[i];
    }

    jc[num_sources] = num_sources;

    mat = malloc(sizeof(spmat));
    mat->jc = jc;
    mat->ir = ir;
    mat->m = getnrows(graph);
    mat->n = num_sources;

    return mat;
}

bitmap* spmat_spmv(const spmat *A, const bitmap *x, const bitmap *v)
{
    bitmap *y;
    long m, n, j, p;

    m = getnrows(A);
    n = getncols(A);

    if (n != bitmap_size(x) || m != bitmap_size(v))
        return NULL;

    if (v && m != bitmap_size(v))
        return NULL;

    y = bitmap_init(m);

    for (j = 0; j < n; ++j)
        if (bitmap_get(x, j))
            for (p = A->jc[j]; p < A->jc[j+1]; ++p)
            {
                if (v)
                {
                    if (!bitmap_get(v, A->ir[p]))
                        bitmap_set(y, A->ir[p]);
                }
                else bitmap_set(y, A->ir[p]);
            }

    return y;
}
