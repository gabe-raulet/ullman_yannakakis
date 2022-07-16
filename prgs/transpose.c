#include "spmat.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <matrix>\n", *argv);
        return -1;
    }

    FILE *fp;
    spmat *A, *AT;

    fp = fopen(argv[1], "r");
    A = spmat_load(fp);
    fclose(fp);

    AT = spmat_transpose(A);
    spmat_write(AT, stdout, 1);
    spmat_free(A);
    spmat_free(AT);

    return 0;
}
