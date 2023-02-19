#include "global.h"

BufferManager::BufferManager()
{
    logger.log("BufferManager::BufferManager");
}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::getPage(string tableName, int pageIndex)
{
    logger.log("BufferManager::getPage");
    string pageName = "../data/temp/"+tableName + "_Page" + to_string(pageIndex);
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    else
        return this->insertIntoPool(tableName, pageIndex);
}

/**
 * @brief Checks to see if a page exists in the pool
 *
 * @param pageName 
 * @return true 
 * @return false 
 */
bool BufferManager::inPool(string pageName)
{
    logger.log("BufferManager::inPool");
    for (auto page : this->pages)
    {
        if (pageName == page.pageName)
            return true;
    }
    return false;
}

/**
 * @brief If the page is present in the pool, then this function returns the
 * page. Note that this function will fail if the page is not present in the
 * pool.
 *
 * @param pageName 
 * @return Page 
 */
Page BufferManager::getFromPool(string pageName)
{
    logger.log("BufferManager::getFromPool");
    for (auto page : this->pages)
        if (pageName == page.pageName)
            return page;
}

/**
 * @brief Inserts page indicated by tableName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure. 
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::insertIntoPool(string tableName, int pageIndex)
{
    BLOCK_ACCESS++;
    logger.log("BufferManager::insertIntoPool");
    if (this->pages.size() >= BLOCK_COUNT)
        pages.pop_front();
    Page page(tableName, pageIndex);
    pages.push_back(page);
    return page;
}

/**
 * @brief The buffer manager is also responsible for writing pages. This is
 * called when new tables are created using assignment statements.
 *
 * @param tableName 
 * @param pageIndex 
 * @param rows 
 * @param rowCount 
 */
void BufferManager::writePage(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("BufferManager::writePage");
    Page page(tableName, pageIndex, rows, rowCount);
    page.writePage();
}

/**
 * @brief Deletes file names fileName
 *
 * @param fileName 
 */
void BufferManager::deleteFile(string fileName)
{
    
    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFile: Err");
    else logger.log("BufferManager::deleteFile: Success");
}

/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the tableName and pageIndex.
 *
 * @param tableName 
 * @param pageIndex 
 */
void BufferManager::deleteFile(string tableName, int pageIndex)
{
    logger.log("BufferManager::deleteFile");
    string fileName = "../data/temp/"+tableName + "_Page" + to_string(pageIndex);
    this->deleteFile(fileName);
}



// ######################################################################## Normal MATRIX BEGIN HERE


void BufferManager::writePageMatrix(string matrixName, int pageIndex_i, int pageIndex_j, vector<vector<int>> rows, int rowCount, int colCount)
{
    logger.log("BufferManager::writePageMatrix");
    PageMatrix pageMatrix(matrixName, pageIndex_i, pageIndex_j, rows, rowCount, colCount);
    pageMatrix.writePageMatrix();
}


PageMatrix BufferManager::getPageMatrix(string matrixName, int pageIndex_i, int pageIndex_j)
{
    logger.log("BufferManager::getPageMarix");
    string pageName = "../data/temp/"+matrixName + "_Page" + to_string(pageIndex_i) + "_" + to_string(pageIndex_j);
    if (this->inPoolMatrix(pageName))
        return this->getFromPoolMatrix(pageName);
    else
        return this->insertIntoPoolMatrix(matrixName, pageIndex_i, pageIndex_j);
}

bool BufferManager::inPoolMatrix(string pageName)
{
    logger.log("BufferManager::inPoolMatrix");
    for (auto page : this->pagesMatrix)
    {
        if (pageName == page.pageName)
            return true;
    }
    return false;
}

PageMatrix BufferManager::getFromPoolMatrix(string pageName)
{
    logger.log("BufferManager::getFromPoolMatrix");
    for (auto page : this->pagesMatrix)
        if (pageName == page.pageName)
            return page;
}

PageMatrix BufferManager::insertIntoPoolMatrix(string matrixName, int pageIndex_i, int pageIndex_j)
{
    logger.log("BufferManager::insertIntoPoolMatrix");
    while(this->pagesMatrixSparseR.size())
        pagesMatrixSparseR.pop_front();
    while(this->pagesMatrixSparseVC.size())
        pagesMatrixSparseVC.pop_front();
    

    if (this->pagesMatrix.size() >= BLOCK_COUNT)
        pagesMatrix.pop_front();
    PageMatrix page(matrixName, pageIndex_i, pageIndex_j);
    pagesMatrix.push_back(page);
    return page;
}

void BufferManager::deleteFileMatrix(string fileName)
{
    
    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFileMatrix: Err");
    else logger.log("BufferManager::deleteFileMatrix: Success");
}

void BufferManager::deleteFileMatrix(string matrixName, int pageIndex_i, int pageIndex_j)
{
    logger.log("BufferManager::deleteFileMatrix");
    string fileName = "../data/temp/"+matrixName + "_Page" + to_string(pageIndex_i) + "_" + to_string(pageIndex_j);
    this->deleteFileMatrix(fileName);
}

// ######################################################################## Sparse VC BEGIN HERE

void BufferManager::writePageSparseVC(string matrixName, int pageIndex, vector<pair<int,int>> VC, int pairsInThisBlock)
{
    logger.log("BufferManager::writePageSparseVC");
    PageMatrixSparseVC pageMatrix(matrixName, pageIndex,  VC, pairsInThisBlock);
    pageMatrix.writePageSparseVC();
}

PageMatrixSparseVC BufferManager::getPageSparseVC(string matrixName, int pageIndex)
{
    logger.log("BufferManager::getPageSparseVC");
    string pageName = "../data/temp/"+ matrixName + "_PageMatrixSparseVC_" + to_string(pageIndex);
    if (this->inPoolSparseVC(pageName))
        return this->getFromPoolSparseVC(pageName);
    else
        return this->insertIntoPoolSparseVC(matrixName, pageIndex);
}

bool BufferManager::inPoolSparseVC(string pageName)
{
    logger.log("BufferManager::inPoolSparseVC");
    for (auto page : this->pagesMatrixSparseVC)
    {
        if (pageName == page.pageName)
            return true;
    }
    return false;
}

PageMatrixSparseVC BufferManager::getFromPoolSparseVC(string pageName)
{
    logger.log("BufferManager::getFromPoolSparseVC");
    for (auto page : this->pagesMatrixSparseVC)
        if (pageName == page.pageName)
            return page;
}

PageMatrixSparseVC BufferManager::insertIntoPoolSparseVC(string matrixName, int pageIndex)
{
    logger.log("BufferManager::insertIntoPoolSparseVC");
    while(this->pagesMatrix.size())
        pagesMatrix.pop_front();

    if (this->pagesMatrixSparseVC.size() >= BLOCK_COUNT/2)
        pagesMatrixSparseVC.pop_front();

    PageMatrixSparseVC page(matrixName, pageIndex);
    pagesMatrixSparseVC.push_back(page);
    return page;
}

void BufferManager::deleteFileSparseVC(string fileName)
{
    
    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFileSparseVC: Err");
    else logger.log("BufferManager::deleteFileSparseVC: Success");
}

void BufferManager::deleteFileSparseVC(string matrixName, int pageIndex)
{
    logger.log("BufferManager::deleteFileSparseVC");
    string fileName = "../data/temp/"+matrixName + "_PageMatrixSparseVC_" + to_string(pageIndex);
    this->deleteFileSparseVC(fileName);
}



// ######################################################################## Sparse R BEGIN HERE


void BufferManager::writePageSparseR(string matrixName, int pageIndex, vector<int> R, int elesInThisBlock)
{
    logger.log("BufferManager::writePageSparseR");
    PageMatrixSparseR pageMatrix(matrixName, pageIndex,  R, elesInThisBlock);
    pageMatrix.writePageSparseR();
}

PageMatrixSparseR BufferManager::getPageSparseR(string matrixName, int pageIndex)
{
    logger.log("BufferManager::getPageSparseR");
    string pageName = "../data/temp/"+matrixName + "_PageMatrixSparseR_" + to_string(pageIndex);
    if (this->inPoolSparseR(pageName))
        return this->getFromPoolSparseR(pageName);
    else
        return this->insertIntoPoolSparseR(matrixName, pageIndex);
}

bool BufferManager::inPoolSparseR(string pageName)
{
    logger.log("BufferManager::inPoolSparseR");
    for (auto page : this->pagesMatrixSparseR)
    {
        if (pageName == page.pageName)
            return true;
    }
    return false;
}

PageMatrixSparseR BufferManager::getFromPoolSparseR(string pageName)
{
    logger.log("BufferManager::getFromPoolSparseR");
    for (auto page : this->pagesMatrixSparseR)
        if (pageName == page.pageName)
            return page;
}

PageMatrixSparseR BufferManager::insertIntoPoolSparseR(string matrixName, int pageIndex)
{
    logger.log("BufferManager::insertIntoPoolSparseR");
    while(this->pagesMatrix.size())
    {
        pagesMatrix.pop_front();
    }
    if (this->pagesMatrixSparseR.size() >= BLOCK_COUNT/2)
        pagesMatrixSparseR.pop_front();
    PageMatrixSparseR page(matrixName, pageIndex);
    pagesMatrixSparseR.push_back(page);
    return page;
}

void BufferManager::deleteFileSparseR(string fileName)
{
    
    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFileSparseR: Err");
    else logger.log("BufferManager::deleteFileSparseR: Success");
}

void BufferManager::deleteFileSparseR(string matrixName, int pageIndex)
{
    logger.log("BufferManager::deleteFileSparseR");
    string fileName = "../data/temp/"+matrixName + "_PageMatrixSparseR_" + to_string(pageIndex);
    this->deleteFileSparseR(fileName);
}

