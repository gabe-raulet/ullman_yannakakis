#include "spmat.h"
#include "bitmap.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "usage: %s [graph.mtx] [levels.txt] [source]\n", *argv);
        return -1;
    }

    const char *graph_filename = argv[1];
    const char *levels_filename = argv[2];
    long source = atoi(argv[3]);

    FILE *fp;
    spmat *mat, *A;
    bitmap *f, *v, *fs;
    long n, i, *w, level;

    fp = fopen(graph_filename, "r");
    mat = spmat_read(fp);
    fclose(fp);

    A = spmat_transpose(mat);
    spmat_free(mat);

    n = getnrows(A);
    f = bitmap_init(n);
    v = bitmap_init(n);
    w = malloc(n * sizeof(long));

    for (i = 0; i < n; ++i)
        w[i] = -1;

    bitmap_set(f, source);
    w[source] = 0;

    level = 0;

    while (!bitmap_empty(f))
    {
        ++level;
        bitmap_union_update(v, f);
        fs = spmat_spmv(A, f, v);
        bitmap_apply(fs, w, level);
        bitmap_free(f);
        f = fs;
        fs = NULL;
    }

    fp = fopen(levels_filename, "w");

    for (i = 0; i < n; ++i)
        fprintf(fp, "%ld %ld\n", i, w[i]);

    fclose(fp);

    free(w);
    bitmap_free(f);
    bitmap_free(v);
    spmat_free(A);

    return 0;
}
