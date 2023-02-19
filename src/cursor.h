#include"bufferManager.h"
/**
 * @brief The cursor is an important component of the system. To read from a
 * table, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 */
class Cursor{
    public:
    Page page;
    int pageIndex;
    string tableName;
    int pagePointer;

    public:
    Cursor(string tableName, int pageIndex);
    vector<int> getNext();
    void nextPage(int pageIndex);
};

class CursorMatrix{
    public:
    PageMatrix pageMatrix;
    int pageIndex_i;
    int pageIndex_j;
    string matrixName;
    int pagePointer;

    public:
    CursorMatrix(string matrixName, int pageIndex_i, int pageIndex_j);
    vector<int> getNextMatrix();
    void nextPageMatrix(int pageIndex_i, int pageIndex_j, int p);
};

class CursorMatrixSparseVC{
    public:
    PageMatrixSparseVC pageMatrixSparseVC;
    int pageIndex;
    string matrixName;
    int pagePointer;

    public:
    CursorMatrixSparseVC(string matrixName, int pageIndex);
    pair<int, int> getNextPair();
    void nextPageVC(int pageIndex);
};

class CursorMatrixSparseR{
    public:
    PageMatrixSparseR pageMatrixSparseR;
    int pageIndex;
    string matrixName;
    int pagePointer;

    public:
    CursorMatrixSparseR(string matrixName, int pageIndex);
    int getNextElement();
    void nextPageR(int pageIndex);
};