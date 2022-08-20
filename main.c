#include "spmat.h"
#include "bitmap.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *fp = fopen("graphs/enron_giant.mtx", "r");
    spmat *mat = spmat_read(fp);
    fclose(fp);

    spmat *A = spmat_transpose(mat);
    spmat_free(mat);

    long source = 0;

    long n = getnrows(A);
    bitmap *f = bitmap_init(n);
    bitmap *v = bitmap_init(n);
    long *w = malloc(n * sizeof(long));

    for (long i = 0; i < n; ++i)
        w[i] = -1;

    bitmap_set(f, source);
    w[source] = 0;

    long level = 0;

    while (!bitmap_empty(f))
    {
        ++level;
        bitmap_union_update(v, f);
        bitmap *fs = spmat_spmv(A, f, v);
        bitmap_apply(fs, w, level);
        bitmap_free(f);

        f = fs;
        fs = NULL;
    }

    for (long i = 0; i < n; ++i)
        printf("%ld %d\n", i, w[i]);

    return 0;
}
