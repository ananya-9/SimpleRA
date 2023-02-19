# Matrix Implementation

## Table of contents: 
- [Matrix Implementation](#matrix-implementation)
  - [Table of contents:](#table-of-contents)
  - [Ideation](#ideation)
  - [Page Layout](#page-layout)
    - [Regular Matrices](#regular-matrices)
  - [Compression Technique Used and Corresponding Page Layout](#compression-technique-used-and-corresponding-page-layout)
    - [Compression Technique](#compression-technique)
    - [Page Layout used for CSR](#page-layout-used-for-csr)
  - [Compression Ratio as function of percentage of sparseness](#compression-ratio-as-function-of-percentage-of-sparseness)
  - [In-Place Transpose Operation](#in-place-transpose-operation)
    - [Transpose Feature Implementation for Regular Matrices](#transpose-feature-implementation-for-regular-matrices)
    - [Transpose Feature Implementation for Sparse Matrices](#transpose-feature-implementation-for-sparse-matrices)

## Ideation

We are given with large square matrices *n x n (n &le; 10<sup>5</sup>)* containing unsigned integers, to be stored in the simulated DBMS. The challenge is to load the matrix in such a way, that none of the *Pages* created exceed the maximum size i.e. *8 KB*. Looking at the maximum size of the matrix, there might be some cases where we cannot even store one whole row in a single page as it might exceed MaxBlockSize i.e. *8 KB*. 

The approach we have used in implementation of Matrix is to divide the larger square matrix *(NxN)* into smaller square sub-matrices of definete size *(MxM)*. 

Size of the smaller matrix *(MxM)* depends on the BLOCK_SIZE and can be calculated as: <br>`M = (floor)sqrt((BLOCK_SIZE*1024)/(sizeof(int)))`

We chose `BLOCK_SIZE = 1 KB`, which gives *M = 16*. 

If one chooses `BLOCK_SIZE = 256Bytes or 0.25 KB`, the size of sub-matrices comes out to be 8. As the sub-matrix size decreases, the total page count increases and hence number of Page accesses increases, which results in time inefficiency. 

This approach of dividing the matrix into smaller submarix makes the `TRANSPOSE` functionality very effecient and intiutive. A row-major store of the large matrix, would be highly inefficient to be transposed in-place as it would require much more Page accesses than we require now. 

---


## Page Layout 


### Regular Matrices

Each page stores a submatrix of maximum size *16 x 16*.

> Considering an *18 x 18* sized matrix,

![](https://i.imgur.com/Iv27IC2.png)

> Each page has an index to indicate submatrix row and submatrix column.

![](https://i.imgur.com/yeX0FLR.png)

---
## Compression Technique Used and Corresponding Page Layout 

### Compression Technique
A sparse matrix is a matrix with count of `zero elements > 0.6 * size of matrix`. With this being known, we can compress the memory required to store a given matrix by only storing the data of non-zero elements of the matrix. 

![](https://matteding.github.io/images/sparse_dense.gif)

The compression technique used here is ***Compressed Sparse Row (CSR)*** 

#### For Example : <!-- omit in toc -->
![](https://wikimedia.org/api/rest_v1/media/math/render/svg/d3ad9e3299e6c5619c049d4518f82bad916960e6)

   ```
   V         = [ 5 8 3 6 ]
   COL_INDEX = [ 0 1 2 1 ]
   ROW_INDEX = [ 0 1 2 3 4 ]
  ```

### Page Layout used for CSR 

**VC Vector** Each non zero element of the matrix along with its column index is stored in the pages for VC (ValueColumn). The total number of pairs in VC for a matrix is the total number of non-zero elements in that matrix.

A single page of VC can contain maximum 128 non zero values and 128 corresponding column indices. The pages are indexed sequentially, as the elements are read in a row-major pattern.


**The R vector** contains cumulative number of non-zero elements upto (not including) the i-th row. The size of R vector is N+1. It is defined by the recursive relation : 
- R[0] = 0
- R[i] = R[i-1] + no of non-zero elements in the (i-1) th row of the Matrix

A single page corresponding to R can store 256 elements at maximum. 
*N = Size of matrix*




---
## Compression Ratio as function of percentage of sparseness


The sparsity of the matrix is the measure of emptiness or sparseness of a given matrix. 

* Mathematically,<br>
Sparsity = Number of zeroes present in Matrix /  Total No of Elements 
<br>Sparsity  = 1 – NNZ/N<sup>2</sup> (For a square matrix of size N)
- If ***no compression*** technique is used, then every element of the matrix is stored into memory. Hence space required = **N<sup>2</sup> x sizeof(uint)**
* Where as, in case of **CSR**, space required = **[(2 x NNZ) + (N+1)] x sizeof(uint)**

- CSR matrices are memory efficient as long as space `NNZ < (N * (N-1) - 1)/2`.

* **Percentage of sparseness ( P )** =  Sparsity * 100 =  *(1 – NNZ/N<sup>2</sup>) \* 100*
<br>Non-Zero elements in terms of P  ->   **NNZ** = *(1 - P / 100) * N<sup>2</sup>*

* **Compression Ratio** [(WikiSource)](https://en.wikipedia.org/wiki/Data_compression_ratio)
= Memory required without compression/ Memory required after compression
<br>= *(N<sup>2</sup>)/(2NNZ + N + 1)*
<br>= *(N<sup>2</sup>)/{(2 \* (1 - P / 100) \* N<sup>2</sup>) + N + 1}* 

    
Solving the above, we get: 

![](https://i.imgur.com/3z7Elyd.png)

- For a given matrix size N, as P increases Compression Ratio increases, i.e., when there are more zeroes, the memory required after compression is lower.


*N = Size of matrix*<br> *NNZ = Number of non-zero elements in the matrix*


---
## In-Place Transpose Operation


### Transpose Feature Implementation for Regular Matrices

The **TRANSPOSE** command calls the transpose executor, which checks for a matrix of the given name in the `matrixCatalogue`, and calls the `matrix.transpose()` function only if the m,atrix is present.

The `transpose()` function works as follows:

1. Loop `pageIndex_i` from *0* to *number of rows of submatrices(blocks)* and loop `pageIndex_j` from *0* to *number of columns of submatrices* (nested loops). 
2. For non-diagonal matrices, i.e. *pageIndex_i != pageIndex_j*, in each iteration: 
    * Get the page corresponding to `(pageIndex_i, pageIndex_j)` and the page corresponding `(pageIndex_j, pageIndex_i)` page.
    * Retrieve the matrices from each of these pages.
    * Transpose each matrix internally.
    * Swap these transposed matrices within their pages, i.e., write the second matrix into the first page and the first page into the second matrix.
3. For diagonal matrices, i.e.,   *pageIndex_i == pageIndex_j* , in each iteration: 
    * Get the `(pageIndex_i, pageIndex_j)` page.
    * Retrieve the matrix from this page.
    * Transpose the matrix internally.
    * Write the matrix back to the page.


### Transpose Feature Implementation for Sparse Matrices
 We chose to implement **lazy transpose**, i.e., we store a boolean flag `isTranspose` in Matrix which indicates whether the matrix is to be transposed or not. The value of this flag is set when the matrix is loaded to the system using the `isSparse()` function call. 
 
 When the **PRINT** or **EXPORT** commands are called, the matrix is read from the pages differently based on value of `isTranspose`.
 
If `isTranspose` is `False`:
*  Get data from VC pages sequentially (as they are already stored row wise) and output to the correct row based on data from R pages.

If `isTranspose` is `True`:

>implementation follows attached image

![](https://i.imgur.com/Sz0pfCL.jpg)
![](https://i.imgur.com/iNJ3wtx.jpg)
![](https://i.imgur.com/Wn8MFId.jpg)
![](https://i.imgur.com/VswwOOx.jpg)

---


