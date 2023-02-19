#include "global.h"

Cursor::Cursor(string tableName, int pageIndex)
{
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getPage(tableName, pageIndex);
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> Cursor::getNext()
{
    logger.log("Cursor::getNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if(result.empty()){
        tableCatalogue.getTable(this->tableName)->getNextPage(this);
        if(!this->pagePointer){
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}
/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void Cursor::nextPage(int pageIndex)
{
    logger.log("Cursor::nextPage");
    this->page = bufferManager.getPage(this->tableName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}

// ######################################################################## NORMAL MATRIX BEGINS HERE


CursorMatrix::CursorMatrix(string matrixName, int pageIndex_i, int pageIndex_j)
{
    logger.log("CursorMatrix::CursorMatrix");
    this->pageMatrix = bufferManager.getPageMatrix(matrixName, pageIndex_i, pageIndex_j);
    this->pagePointer = 0;
    this->matrixName = matrixName;
    this->pageIndex_i = pageIndex_i;
    this->pageIndex_j = pageIndex_j;
}

vector<int> CursorMatrix::getNextMatrix()
{
    logger.log("CursorMatrix::getNextMatrix");
    vector<int> result;
    result = this->pageMatrix.getRowMatrix(this->pagePointer);
    if(result.empty()){
        matrixCatalogue.getMatrix(this->matrixName)->getNextPageMatrix(this);
        if(!this->pagePointer){
            result = this->pageMatrix.getRowMatrix(this->pagePointer);
            this->pagePointer++;
        }
    }
    matrixCatalogue.getMatrix(this->matrixName)->getNextPageMatrix(this);
   
    return result;
}

void CursorMatrix::nextPageMatrix(int pageIndex_i, int pageIndex_j, int p) 
{
    logger.log("CursorMatrix::nextPageMatrix");
    this->pageMatrix = bufferManager.getPageMatrix(this->matrixName, pageIndex_i, pageIndex_j);
    this->pageIndex_i = pageIndex_i;
    this->pageIndex_j = pageIndex_j;
    this->pagePointer = p;
}



// ######################################################################## SPARSE VC BEGINS HERE

CursorMatrixSparseVC::CursorMatrixSparseVC(string matrixName, int pageIndex)
{
    logger.log("CursorMatrixSparseVC::CursorMatrixSparseVC");
    this->pageMatrixSparseVC = bufferManager.getPageSparseVC(matrixName, pageIndex);
    this->pagePointer = 0;
    this->matrixName = matrixName;
    this->pageIndex= pageIndex;
}


pair<int, int> CursorMatrixSparseVC::getNextPair()
{
    logger.log("CursorMatrixSparseVC::getNextPair");
    pair<int ,int> result = this->pageMatrixSparseVC.getPair(this->pagePointer);
    this->pagePointer++;
    if(result.first==-1){
        matrixCatalogue.getMatrix(this->matrixName)->getNextPageSparseVC(this);
        if(!this->pagePointer){
            result = this->pageMatrixSparseVC.getPair(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}

void CursorMatrixSparseVC::nextPageVC(int pageIndex)
{
    logger.log("CursorMatrixSparseVC::nextPageVC");
    this->pageMatrixSparseVC = bufferManager.getPageSparseVC(this->matrixName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}



// ######################################################################## SPARSE R BEGINS HERE

CursorMatrixSparseR::CursorMatrixSparseR(string matrixName, int pageIndex)
{
    logger.log("CursorMatrixSparseR::CursorMatrixSparseR");
    this->pageMatrixSparseR = bufferManager.getPageSparseR(matrixName, pageIndex);
    this->pagePointer = 0;
    this->matrixName = matrixName;
    this->pageIndex= pageIndex;
}


int CursorMatrixSparseR::getNextElement()
{
    logger.log("CursorMatrixSparseR::getNextElement");
    int result = this->pageMatrixSparseR.getElement(this->pagePointer);
    this->pagePointer++;
    if(result==-1){
        matrixCatalogue.getMatrix(this->matrixName)->getNextPageSparseR(this);
        if(!this->pagePointer){
            result = this->pageMatrixSparseR.getElement(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}

void CursorMatrixSparseR::nextPageR(int pageIndex)
{
    logger.log("CursorMatrixSparseR::nextPageR");
    this->pageMatrixSparseR = bufferManager.getPageSparseR(this->matrixName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}