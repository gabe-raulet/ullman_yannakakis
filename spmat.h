#pragma once

#include <stdio.h>
#include "bitmap.h"

typedef struct
{
    long m;    /* number of rows */
    long n;    /* number of columns */
    long *jc;  /* column pointers, size n+1 */
    long *ir;  /* row indices, size jc[n] */
} spmat;

#define getnrows(mat) ((mat)->m)
#define getncols(mat) ((mat)->n)
#define getnnz(mat) ((mat)->jc[(mat)->n])

/**
 * @func spmat_init
 *
 * @param nrows   [long]        - number of rows in sparse matrix
 * @param ncols   [long]        - number of columns in sparse matrix
 * @param nnz     [long]        - number of nonzeros in sparse matrix
 * @param rowidxs [const long*] - @nnz-size array of row indices
 * @param colidxs [const long*] - @nnz-size array of column indices
 *
 * @return [spmat*] - a new spmat object initialized with the above parameters
 *
 * The returned spmat object will have dimensions @nrows-by-@ncols, with @nnz
 * nonzeros. @rowidxs and @colidxs are each arrays of @nnz longs, such that
 * the returned matrix has a nonzero (i,j) if and only if there exists 0 <= k < @nnz
 * for which @rowidxs[k] = i and @colidxs[k] = j.
 **/
spmat* spmat_init(long nrows, long ncols, long nnz, const long *rowidxs, const long *colidxs);

/**
 * @func spmat_read
 *
 * @param f [FILE *] - file pointer to input file
 *
 * @return [spmat* - a new spmat object initialized with the contents
 *                   of the input file
 *
 * The expected input file format has a single line at the top defining the
 * dimensions and number of nonzeros in the matrix:
 *
 *  NUM_ROWS NUM_COLS NUM_NONZEROS
 *
 * Following this line are @NUM_NONZEROS lines of the following form:
 *
 *  ROW_INDEX COL_INDEX
 *
 * All lines should contain longs separated by whitespace.
 **/
spmat* spmat_read(FILE *f);

/**
 * @func spmat_copy
 *
 * @param mat [const spmat*] - an initialized spmat object
 *
 * @return [spmat*] - a new spmat object copied from the input spmat
 *                    object @mat
 **/
spmat* spmat_copy(const spmat *mat);

/**
 * @func spmat_free
 *
 * @param mat [spmat*] - an initialized spmat object
 *
 * Frees all the memory associated with an initialized spmat object.
 **/
void spmat_free(spmat *mat);


/**
 * @func spmat_transpose
 *
 * @param mat [const spmat*] - an initialized spmat object
 *
 * @return [spmat*] - a new spmat object which is the transpose
 *                    of @mat
 **/
spmat* spmat_transpose(const spmat *mat);


/**
 * @func spmat_spgemm
 *
 * @param A [const spmat*] - an initialized spmat object
 * @param B [const spmat*] - an initialized spmat object (can be the same as @A)
 *
 * @return [spmat*] - a new spmat object which is the matrix-matrix product
 *                    of @A and @B
 *
 * A null pointer is returned upon failure of any kind, including mismatching
 * dimensions.
 **/
spmat* spmat_spgemm(const spmat *A, const spmat *B);

/**
 * @func spmat_add
 *
 * @param A [const spmat*] - an initialized spmat object
 * @param B [const spmat*] - an initialized spmat object (can be the same as @A)
 *
 * @return [spmat*] - a new spmat object which is the matrix-matrix sum
 *                    of @A and @B
 *
 * A null pointer is returned upon failure of any kind, including mismatching
 * dimensions.
 **/
spmat* spmat_add(const spmat *A, const spmat *B);

/**
 * @func spmat_diff
 *
 * @param A [const spmat*] - an initialized spmat object
 * @param B [const spmat*] - an initialized spmat object (CANNOT be the same as @A)
 *
 * @return [spmat*] - a new spmat object which is contains only the nonzeros
 *                    in @A that are not in @B
 *
 * A null pointer is returned upon failure of any kind, including mismatching
 * dimensions.
 **/
spmat* spmat_diff(const spmat *A, const spmat *B);

/**
 * @func spmat_init_msbfs
 *
 * @param graph       [const spmat*] - an initialized spmat object representing a graph
 * @param sources     [const long*]  - an array of @num_sources longs representing ms-bfs source vertices
 * @param num_sources [long]         - the number of source vertices
 *
 * @return [spmat*] - a new spmat object representing the frontier for ms-bfs.
 *
 * A null pointer is returned upon failure of any kind.
 **/
spmat* spmat_init_msbfs(const spmat *graph, const long *sources, long num_sources);

/**
 * @func spmat_spmv
 *
 * @param A         [const spmat*]  - an initialized spmat object representing a graph
 * @param x         [const bitmap*] - an initialized bitmap object representing a dense vector
 * @param v         [const bitmap*] - an initialized bitmap object which is a mask
 *                                    on the output vector, NULL if no mask
 *
 * @return [bitmap*] - a new bitmap object representing the matrix-vector product
 *                     of @A and @x
 *
 *
 * All locations where v has a set bit will be left unset on the output bitmap.
 * A null pointer is returned upon failure of any kind, including mismatching
 * dimensions.
 **/
bitmap* spmat_spmv(const spmat *A, const bitmap *x, const bitmap *v);
