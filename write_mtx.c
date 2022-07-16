#include "spmat.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    spmat *A = spmat_load(stdin);
    spmat_write(A, stdout, 1);
    spmat_free(A);

    return 0;
}
