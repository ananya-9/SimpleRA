#include "table.h"
/**
 * @brief The Matrix class holds all information related to a loaded matrix. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a matrix object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT). 
 *
 */
class Matrix
{
    vector<unordered_set<int>> distinctValuesInColumns;

public:
    string sourceFileName = "";
    string matrixName = "";
    vector<int> columns; //implicit column numbers- like zero indexing
    vector<uint> distinctValuesPerColumnCount;
    uint matrixSize = 0;
    long long int rowCount = 0;
    long long int colCount = 0;
    uint blockCount = 0; //number of pages = 16
    uint blockRoot = 0;
    //block size= 1.024 kB -> memory used 1024 
    //number of integers in a page = 1024 / sizeof(int) = 256
    //Rows = root(256) = 16
    uint maxRowsPerBlock = 0;
    uint maxColPerBlock = 0;
    vector<uint> rowsPerBlockCount;
    vector<uint> colsPerBlockCount;
    bool indexed = false;
    string indexedColumn = "";
    IndexingStrategy indexingStrategy = NOTHING;

    // ######################################################################## Sparse variables
    bool isSparse=false;
    bool isTranspose=false;
    int nz_count=0;
    uint maxPairsPerBlockVC = 0;
    uint maxElePerBlockR = 0;
    vector<uint> pairsPerBlockCountVC;
    vector<uint> elesPerBlockCountR;
    int blockCountVC=0;
    int blockCountR=0;

    
    bool extractColumnNumbers(string sourceFileName);
    bool blockify();
    bool sparsify();
    void updateStatistics(vector<int> row);
    Matrix();
    Matrix(string matrixName);
    bool load();
    void print();
    void print_sparse();
    void print_sparse_transpose();
    void makePermanent();
    void makePermanent_sparse();
    void makePermanent_sparse_transpose();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    int getColumnIndex(string columnName);
    //void unload();
    void getNextPageMatrix(CursorMatrix *cursorMatrix);
    void transpose_entry();
    void checkisSparse();

    // ######################################################################## Sparse VC begin here
    void getNextPageSparseVC(CursorMatrixSparseVC *cursorMatrixSparseVC);


    // ######################################################################## Sparse R begin Here
    void getNextPageSparseR(CursorMatrixSparseR *cursorMatrixSparseR);



template <typename T>
void writeRowMatrix(vector<T> row, ostream &fout)
{
    logger.log("Matrix::printRow");
    for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
    {
        if (columnCounter != 0 )
            fout << ",";
        fout << row[columnCounter];
    }
}

template <typename T>
void writeRowMatrixTerminal(vector<T> row, ostream &fout)
{
    logger.log("Matrix::printRow");
    for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
    {
        if (columnCounter != 0 )
            fout << "\t";
        fout << row[columnCounter];
    }
}

template <typename T>
void writeRowMatrix(vector<T> row)
{
    logger.log("Matrix::printRow");
    ofstream fout(this->sourceFileName, ios::app);
    this->writeRowMatrix(row, fout);
    fout.close();
}
};