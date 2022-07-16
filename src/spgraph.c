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

    char line[1024];

    while (fgets(line, 1024, f))
    {
        assert(sscanf(line, "%lld %lld", &u, &v)==2);
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

void spgraph_write(spgraph *g, FILE *f, int header)
{
    index_t v, p, nadj, *rows;

    if (header) fprintf(f, "%lld %lld %lld\n", g->n, g->n, g->jc[g->n]);

    rows = NULL;

    for (v = 0; v < g->n; ++v)
    {
        nadj = g->jc[v+1] - g->jc[v];
        if (nadj > 0)
        {
            rows = realloc(rows, nadj * sizeof(index_t));
            memcpy(rows, g->ir + g->jc[v], nadj);
            qsort(rows, nadj, sizeof(index_t), index_compare);
            for (p = g->jc[v]; p < g->jc[v+1]; ++p)
            {
                index_t u = g->ir[p];
                fprintf(f, "%lld %lld\n", u+1, v+1);
            }
        }
    }

    free(rows);
}
