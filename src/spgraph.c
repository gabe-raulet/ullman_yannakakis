#include "spgraph.h"

spgraph *spgraph_init(index_t n, index_t nz, index_t *ir, index_t *jc)
{
    index_t k, p, nzcnt, *colcnts;
    spgraph *g;

    g = malloc(sizeof(spgraph));
    g->n = n;
    g->ir = malloc(nz * sizeof(index_t));
    g->jc = calloc(n+1, sizeof(index_t));
    colcnts = calloc(n+1, sizeof(index_t));

    for (k = 0; k < nz; ++k)
        colcnts[jc[k]]++;

    nzcnt = 0;
    for (k = 0; k < n; ++k)
    {
        g->jc[k] = nzcnt;
        nzcnt += colcnts[k];
        colcnts[k] = g->jc[k];
    }
    assert(nzcnt == nz);
    g->jc[n] = nzcnt;

    for (k = 0; k < nz; ++k)
    {
        p = colcnts[jc[k]]++;
        g->ir[p] = ir[k];
    }
    free(colcnts);
    return g;
}

void spgraph_free(spgraph *g)
{
    if (!g) return;
    g->n = 0;
    free(g->jc);
    free(g->ir);
    free(g);
}

spgraph *spgraph_load(FILE *f, int directed)
{
    index_t u, v, n, nz, nzmax, *ir, *jc;
    spgraph *g;

    if (!f || !directed) return NULL;

    n = nz = 0;
    nzmax = 16;
    ir = calloc(nzmax, sizeof(index_t));
    jc = calloc(nzmax, sizeof(index_t));

    while (fscanf(f, "%lld %lld\n", &u, &v) == 2)
    {
        ir[nz] = u-1;
        jc[nz++] = v-1;
        n = MAX(n, MAX(u, v));

        if (nz >= nzmax)
        {
            nzmax *= 2;
            ir = realloc(ir, nzmax * sizeof(index_t));
            jc = realloc(jc, nzmax * sizeof(index_t));
        }
    }

    g = spgraph_init(n, nz, ir, jc);
    free(ir);
    free(jc);
    return g;
}
