#include "spmat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>

struct timespec tstart, tstop, tresult;

#define timer_start() clock_gettime(CLOCK_MONOTONIC, &tstart)
#define timer_stop() clock_gettime(CLOCK_MONOTONIC, &tstop)
#define timer_elapsed() ((double)(tstop.tv_sec-tstart.tv_sec) + (double)(tstop.tv_nsec-tstart.tv_nsec)*1.0e-9)
void vector_write(const index_t *v, FILE *f, index_t n, int indices);
void vector_set(index_t *v, const index_t *x, index_t val, index_t n);
index_t vector_nzs(const index_t *v, index_t n);
index_t *bfs(const spmat *A, index_t source);

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "usage: %s <mtx> <paths prefix> [... source vertices ...]\n", *argv);
        return -1;
    }

    const char *mtx_filename = argv[1];
    const char *paths_filename = argv[2];

    fprintf(stderr, "Started reading '%s'\n", mtx_filename);

    timer_start();
    FILE *fp = fopen(mtx_filename, "r");
    spmat *A = spmat_load(fp, 1);
    fclose(fp);
    timer_stop();

    fprintf(stderr, "Finished reading '%s' (%" PRId64 " rows, %" PRId64 " columns, %" PRId64 " nonzeros) [%f secs]\n\n",
        mtx_filename, A->m, A->n, A->jc[A->n], timer_elapsed());

    char outfilename[1024];

    for (int argidx = 3; argidx < argc; ++argidx)
    {
        index_t source = strtol(argv[argidx], NULL, 10) - 1;
        fprintf(stderr, "Started bfs from vertex %" PRId64 "\n", source+1);
        timer_start();
        index_t *paths = bfs(A, source);
        timer_stop();
        fprintf(stderr, "Finished bfs [%f secs]\n", timer_elapsed());

        index_t explored = 0;
        for (index_t i = 0; i < A->n; ++i)
            if (paths[i] != -1) ++explored;

        fprintf(stderr, "%" PRId64 "/%" PRId64 " vertices reachable from vertex %" PRId64 "\n\n", explored, A->n, source+1);

        sprintf(outfilename, "%s.%" PRId64 ".txt", paths_filename, source+1);
        fp = fopen(outfilename, "w");
        vector_write(paths, fp, A->n, 1);
        fclose(fp);
        free(paths);
    }

    spmat_free(A);
    return 0;
}

void vector_write(const index_t *v, FILE *f, index_t n, int indices)
{
    index_t i;
    for (i = 0; i < n; ++i)
    {
        if (indices) fprintf(f, "%" PRId64 " %" PRId64 "\n", i+1, v[i]);
        else fprintf(f, "%" PRId64 "\n", v[i]);
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

    n = A->m;

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

    fprintf(stderr, "Performed %" PRId64 " bfs iterations\n", level);

    free(visited);
    free(frontier);
    spmat_free(AT);

    return levels;
}
