#include "spmat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void vector_write(const index_t *v, FILE *f, index_t n, int indices);
void vector_set(index_t *v, const index_t *x, index_t val, index_t n);
index_t vector_nzs(const index_t *v, index_t n);
index_t *bfs(const spmat *A, index_t source);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <mtx> <src>\n", *argv);
        return -1;
    }

    const char *mtx_filename = argv[1];
    const index_t source = strtol(argv[2], NULL, 10) - 1;

    FILE *fp = fopen(mtx_filename, "r");
    spmat *A = spmat_load(fp, 1);
    fclose(fp);

    index_t *paths = bfs(A, source);
    vector_write(paths, stdout, A->n, 1);

    free(paths);
    spmat_free(A);
    return 0;
}

void vector_write(const index_t *v, FILE *f, index_t n, int indices)
{
    index_t i;
    for (i = 0; i < n; ++i)
    {
        if (indices) fprintf(f, "%lld %lld\n", i+1, v[i]);
        else fprintf(f, "%lld\n", v[i]);
    }
}

void vector_set(index_t *v, const index_t *x, index_t val, index_t n)
{
    index_t i;
    for (i = 0; i < n; ++i)
        if (x[i]) v[i] = val;
}

index_t vector_nzs(const index_t *v, index_t n)
{
    index_t nz, i;
    nz = 0;
    for (i = 0; i < n; ++i) if (v[i]) nz++;
    return nz;
}

index_t *bfs(const spmat *A, index_t source)
{
    index_t n, level, k, *levels, *visited, *frontier;
    spmat *AT;

    n = A->m;  /* number of vertices */

    assert(A->m == A->n && source >= 0 && source < n);

    visited = calloc(n, sizeof(index_t));
    frontier = calloc(n, sizeof(index_t));
    levels = malloc(n * sizeof(index_t));

    for (k = 0; k < n; ++k) levels[k] = -1;

    AT = spmat_transpose(A);

    frontier[source] = 1;
    levels[source] = level = 0;

    while (vector_nzs(frontier, n) > 0)
    {
        ++level;
        vector_set(visited, frontier, 1, n);
        spmat_spmv_bool(AT, frontier, visited, 1);
        vector_set(levels, frontier, level, n);
    }

    free(visited);
    free(frontier);
    spmat_free(AT);

    return levels;
}
