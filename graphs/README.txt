Graph Statistics
----------------

    graphs/G67.txt:

        * undirected
        * 10000 vertices
        * 20000 edges
        * http://www.cise.ufl.edu/research/sparse/matrices/Gset/G67

    graphs/ibm32.txt:

        * directed
        * 32 vertices
        * 126 edges
        * http://www.cise.ufl.edu/research/sparse/matrices/HB/ibm32

    graphs/gre_115.txt:

        * directed
        * 115 vertices
        * 421 edges
        * http://www.cise.ufl.edu/research/sparse/matrices/HB/gre_115

    graphs/gre_1107.txt:

        * directed
        * 1107 vertices
        * 5664 edges
        * http://www.cise.ufl.edu/research/sparse/matrices/HB/gre_1107

    graphs/hor_131.txt:

        * directed
        * 434 vertices
        * 4710 edges
        * http://www.cise.ufl.edu/research/sparse/matrices/HB/hor_131

Graph Curation Steps
--------------------

    1. Obtain a matrix market file somewhere (preferably from the SuiteSparse collection)
    2. Remove all lines that start with '%'
    3. Remove header listing number of rows/columns/nonzeros
    4. Remove value column (the third one) if present
