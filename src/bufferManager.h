#include"page.h"

/**
 * @brief The BufferManager is responsible for reading pages to the main memory.
 * Recall that large files are broken and stored as blocks in the hard disk. The
 * minimum amount of memory that can be read from the disk is a block whose size
 * is indicated by BLOCK_SIZE. within this system we simulate blocks by
 * splitting and storing the file as multiple files each of one BLOCK_SIZE,
 * although this isn't traditionally how it's done. You can alternatively just
 * random access to the point where a block begins within the same
 * file. In this system we assume that the the sizes of blocks and pages are the
 * same. 
 * 
 * <p>
 * The buffer can hold multiple pages quantified by BLOCK_COUNT. The
 * buffer manager follows the FIFO replacement policy i.e. the first block to be
 * read in is replaced by the new incoming block. This replacement policy should
 * be transparent to the executors i.e. the executor should not know if a block
 * was previously present in the buffer or was read in from the disk. 
 * </p>
 *
 */
class BufferManager{

    deque<Page> pages; 
    bool inPool(string pageName);
    Page getFromPool(string pageName);
    Page insertIntoPool(string tableName, int pageIndex);

    // ######################################################################## MATRIX BEGINS HERE

    deque<PageMatrix> pagesMatrix; 
    bool inPoolMatrix(string pageName);
    PageMatrix getFromPoolMatrix(string pageName);
    PageMatrix insertIntoPoolMatrix(string matrixName, int pageIndex_i, int pageIndex_j);

    
    // ######################################################################## SPARSE VC BEGINS HERE
    deque<PageMatrixSparseVC> pagesMatrixSparseVC;
    bool inPoolSparseVC(string pageName);
    PageMatrixSparseVC getFromPoolSparseVC(string pageName);
    PageMatrixSparseVC insertIntoPoolSparseVC(string matrixName, int pageIndex);

    // ######################################################################## SPARSE R BEGINS HERE
    deque<PageMatrixSparseR> pagesMatrixSparseR; 
    bool inPoolSparseR(string pageName);
    PageMatrixSparseR getFromPoolSparseR(string pageName);
    PageMatrixSparseR insertIntoPoolSparseR(string matrixName, int pageIndex);


    public:
    
    BufferManager();
    Page getPage(string tableName, int pageIndex);
    void writePage(string pageName, vector<vector<int>> rows);
    void deleteFile(string tableName, int pageIndex);
    void deleteFile(string fileName);
    void writePage(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount);

    //################################################## Matrix begin here

    PageMatrix getPageMatrix(string matrixName, int pageIndex_i, int pageIndex_j);
    void deleteFileMatrix(string matrixName, int pageIndex_i, int pageIndex_j);
    void deleteFileMatrix(string fileName);
    void writePageMatrix(string matrixName, int pageIndex_i, int pageIndex_j, vector<vector<int>> rows, int rowCount, int colCount);




    //################################################## Sparse VC Begin here

    PageMatrixSparseVC getPageSparseVC(string matrixName, int pageIndex);
    void deleteFileSparseVC(string matrixName, int pageIndex);
    void deleteFileSparseVC(string fileName);
    void writePageSparseVC(string matrixName, int pageIndex, vector<pair<int,int>> VC, int pairsInThisBlock);
     




    //################################################## Sparse R Begin here

    PageMatrixSparseR getPageSparseR(string matrixName, int pageIndex);
    void deleteFileSparseR(string matrixName, int pageIndex);
    void deleteFileSparseR(string fileName);
    void writePageSparseR(string matrixName, int pageIndex, vector<int> R, int elesInThisBlock);
    
};












