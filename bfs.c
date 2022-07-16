#include "spgraph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "usage: %s <triples.txt> <(un)directed> <bfs source vertex> <path_lengths.txt>\n", *argv);
        return -1;
    }

    const char *triples_filename = argv[1];
    const char *directedness     = argv[2];
    int64_t source_vertex        = strtol(argv[3], NULL, 10);
    const char *paths_filename   = argv[4];

    int directed;

    if (!strcmp(directedness, "directed"))
        directed = 1;
    else if (!strcmp(directedness, "undirected"))
        directed = 0;
    else
    {
        fprintf(stderr, "error: '%s' is not one of {'directed', 'undirected'}\n", directedness);
        return -1;
    }

    FILE *fp = fopen(triples_filename, "r");
    spgraph *g = spgraph_load(fp, directed);
    spgraph_write(g, stdout, 1);

    spgraph_free(g);

    return 0;
}
