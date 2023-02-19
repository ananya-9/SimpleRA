#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    BLOCK_ACCESS++;
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}


//############################################# NORMAL MATRIX BEGIN HERE


PageMatrix::PageMatrix()
{
    this->pageName = "";
    this->matrixName = "";
    this->pageIndex_i = -1;
    this->pageIndex_j = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}
PageMatrix::PageMatrix(string matrixName, int pageIndex_i, int pageIndex_j)
{
    logger.log("PageMatrix::PageMatrix");
    this->matrixName = matrixName;
    this->pageIndex_i = pageIndex_i;
    this->pageIndex_j = pageIndex_j;
    this->pageName = "../data/temp/" + this->matrixName + "_PageMatrix" + to_string(pageIndex_i)+ "_" + to_string(pageIndex_j);
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);
    this->columnCount = matrix.colCount;
    uint maxRowCount = matrix.maxRowsPerBlock;
    

    ifstream fin(pageName, ios::in);
    this->rowCount = matrix.rowsPerBlockCount[pageIndex_i*matrix.blockRoot + pageIndex_j];
    this->columnCount = matrix.colsPerBlockCount[pageIndex_i*matrix.blockRoot + pageIndex_j];
    vector<int> row(this->columnCount, 0);
    this->rows.assign(this->rowCount, row);
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}


vector<int> PageMatrix::getRowMatrix(int rowIndex)
{
    logger.log("PageMatrix::getRowMatrix");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
    {

        return result;
    }
    return this->rows[rowIndex];
}

PageMatrix::PageMatrix(string matrixName, int pageIndex_i, int pageIndex_j, vector<vector<int>> rows, int rowCount, int colCount)
{
    logger.log("PageMatrix::PageMatrix");
    this->matrixName = matrixName;
    this->pageIndex_i = pageIndex_i;
    this->pageIndex_j = pageIndex_j;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = colCount;
    this->pageName = "../data/temp/"+this->matrixName + "_PageMatrix" + to_string(pageIndex_i)+ "_" + to_string(pageIndex_j);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void PageMatrix::writePageMatrix()
{   
    logger.log("PageMatrix::writePageMatrix");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {

        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {

            if (columnCounter != 0)
            {

                fout << " ";
            }
            fout << this->rows[rowCounter][columnCounter];

        }
        fout << endl;

    }
    fout.close();
}

vector<vector<int>> PageMatrix::transpose()
{
    long long int i, j;
    vector<int> row(this->rowCount, 0);
    vector<vector<int>> B(this->columnCount, row);
    for(i = 0; i < this->columnCount; i++)
    {

        for(j = 0; j < this->rowCount; j++)
        {
            B[i][j] = this->rows[j][i];
        }
    }
    return B;
}

//############################################# SPARSE VC BEGIN HERE

PageMatrixSparseVC::PageMatrixSparseVC()
{
    this->pageName = "";
    this->matrixName = "";
    this->pageIndex = -1;
    this->pairsInThisBlock=0;
    this->VC.clear();
}

PageMatrixSparseVC::PageMatrixSparseVC(string matrixName, int pageIndex)
{
    logger.log("PageMatrix::PageMatrixSparseVC");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->matrixName + "_PageMatrixSparseVC_" + to_string(pageIndex);
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);
    this->pairsInThisBlock = matrix.pairsPerBlockCountVC[pageIndex];
    pair<int, int> p={0,0}; 
    this->VC.assign(this->pairsInThisBlock, p);

    ifstream fin(pageName, ios::in);
    int number1, number2;
    for (uint pairCounter = 0; pairCounter < this->pairsInThisBlock; pairCounter++)
    {
        fin >> number1;
        fin >> number2;
        this->VC[pairCounter].first = number1;
        this->VC[pairCounter].second = number2;
    }
    fin.close();
}

PageMatrixSparseVC::PageMatrixSparseVC(string matrixName, int pageIndex, vector<pair<int,int>> VC, int pairsInThisBlock)
{
    logger.log("PageMatrixSparseVC::PageMatrixSparseVC");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->VC = VC;
    this->pairsInThisBlock=pairsInThisBlock;
    this->pageName = "../data/temp/"+this->matrixName + "_PageMatrixSparseVC_" + to_string(pageIndex);
}

void PageMatrixSparseVC::writePageSparseVC()
{
    logger.log("PageMatrixSparseVC::writePageMatrixSparseVC");
    ofstream fout(this->pageName, ios::trunc);

    for(int pairCounter=0; pairCounter<this->pairsInThisBlock; pairCounter++)
    {
        if(pairCounter!=0)
            fout<<" "; 
        fout<< this->VC[pairCounter].first<<" "<<VC[pairCounter].second;
    }

    fout.close();
}

pair<int, int> PageMatrixSparseVC::getPair(int pairIndex)
{
    logger.log("Page::getPair");
    pair<int, int>result;
    result.first=-1;
    result.second=-1;

    if (pairIndex>= this->pairsInThisBlock)
        return result;
    return this->VC[pairIndex];
}

//############################################# SPARSE R BEGIN HERE

PageMatrixSparseR::PageMatrixSparseR()
{
    this->pageName = "";
    this->matrixName = "";
    this->pageIndex = -1;
    this->elesInThisBlock=0;
    this->R.clear();
}


PageMatrixSparseR::PageMatrixSparseR(string matrixName, int pageIndex)
{
    logger.log("PageMatrix::PageMatrixSparseR");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->matrixName + "_PageMatrixSparseR_" + to_string(pageIndex);
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);
    this->elesInThisBlock= matrix.elesPerBlockCountR[pageIndex];
    this->R.assign(this->elesInThisBlock, 0);
    ifstream fin(pageName, ios::in);
    int number;
    for (uint elesCounter = 0; elesCounter < this->elesInThisBlock; elesCounter++)
    {
        fin >> number;
        this->R[elesCounter]= number;
    }
    fin.close();
}


PageMatrixSparseR::PageMatrixSparseR(string matrixName, int pageIndex, vector<int> R, int elesInThisBlock)
{
    logger.log("PageMatrixSparseR::PageMatrixSparseR");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->R = R;
    this->elesInThisBlock=elesInThisBlock;
    this->pageName = "../data/temp/"+this->matrixName + "_PageMatrixSparseR_" + to_string(pageIndex);
}

void PageMatrixSparseR::writePageSparseR()
{
    logger.log("PageMatrixSparseR::writePageMatrixSparseR");
    ofstream fout(this->pageName, ios::trunc);

    for(int elesCounter=0; elesCounter<this->elesInThisBlock; elesCounter++)
    {
        if(elesCounter!=0)
            fout<<" "; 
        fout<< this->R[elesCounter]<<" ";
    }

    fout.close();
}

int PageMatrixSparseR::getElement(int elementIndex)
{
    logger.log("PageMatrixSparseR::getElement");
    int result;
    result=-1;
    if (elementIndex>= this->elesInThisBlock)
        return result;
    return this->R[elementIndex];
}

