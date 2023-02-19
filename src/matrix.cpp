#include "global.h"

/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param matrixName 
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates matrix
 * statistics.
 *
 * @return true if the matrix has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    if (this->extractColumnNumbers(this->sourceFileName))
    {   
        this->checkisSparse();
        if(this->isSparse)
        {
            if(this->sparsify())
                return true;
        }
        else
        {
            if (this->blockify())
                return true;
        }
    }   

    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */


bool Matrix::extractColumnNumbers(string sourceFileName)
{
    logger.log("Matrix::extractColumnNumbers");
    unordered_set<string> columnNumbers;
    fstream fin(sourceFileName, ios::in);
    char c;
    int comma_count=0;
    int check=0;
    if(fin.get(c))
    {
        check=1;
        while(fin.get(c))
        {
            if(c=='\n' or c=='\0')
                break;
            else if(c==',')
            {
                comma_count++;
            }
            else
                continue;
        }
    }
    
    this->matrixSize= comma_count+check;
    fin.close();
    return true;
}

void Matrix::checkisSparse()
{

    logger.log("Matrix::checkisSparse");
    unordered_set<string> columnNumbers;
    fstream fin(sourceFileName, ios::in);
    int zero_count=0;
    string word;
    for(int row_count=0; row_count< this->matrixSize; row_count++)
    {

        for(int col_count=0; col_count < this->matrixSize-1; col_count++)
        {
            getline(fin, word, ',');
            if(stoi(word)==0)
            {

                zero_count++;
            }
        }
        if(row_count!=this->matrixSize-1)
        {
            getline(fin, word, '\n');

            if(stoi(word)==0)
            {

                zero_count++;
            }
        }
        else
        {
            char c;
            c = fin.get();
            if(c=='0')
            {
                zero_count++;
            }
        }
    }
    int min_zeroes = ceil((float)(6*(this->matrixSize*this->matrixSize))/10);
  
    if(zero_count!=1 and this->matrixSize==1)
    {
        return;
    }
    
    if(zero_count>=min_zeroes)
    {
        this->nz_count= this->matrixSize*this->matrixSize- zero_count;
        this->isSparse=true;        
    }
    
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    int sub_matrix_size = sqrt((BLOCK_SIZE*1024)/(sizeof(int)));
    this->maxRowsPerBlock = sub_matrix_size;
    this->maxColPerBlock = sub_matrix_size;
    vector<int> row(this-> maxColPerBlock, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    
    int loop_count_max = ceil(((float)this->matrixSize)/sub_matrix_size);

    for(int loop_count_row=0;loop_count_row<loop_count_max;loop_count_row++)
    {
        int row_max=sub_matrix_size;
        int col_max=sub_matrix_size;
        for(int loop_count_col=0;loop_count_col<loop_count_max;loop_count_col++)
        {
            if(loop_count_row==loop_count_max-1)
            {   
                if(this->matrixSize%sub_matrix_size)
                    row_max = this->matrixSize%sub_matrix_size;
                else
                   row_max = sub_matrix_size;
            }
            if(loop_count_col==loop_count_max-1)
            {   
                if(this->matrixSize%sub_matrix_size)
                    col_max = this->matrixSize%sub_matrix_size;
                else
                    col_max = sub_matrix_size; 
            }



            for(int row_count=0; row_count < row_max; row_count++)
            {

                for(int comma_traverse=1; comma_traverse<=(sub_matrix_size*(loop_count_col));comma_traverse++)
                {
                    getline(fin, word, ',');
                }   

                if(loop_count_col==loop_count_max-1)
                {
                    for(int col_count=0; col_count < col_max-1; col_count++)
                    {
                        getline(fin, word, ',');
                        row[col_count] = stoi(word);
                        rowsInPage[pageCounter][col_count] = row[col_count];
                    }
                    getline(fin, word, '\n');
                    row[col_max-1] = stoi(word);
                    rowsInPage[pageCounter][col_max-1] = row[col_max-1];
                    pageCounter++;    
                }
                else
                {
                    for(int col_count=0; col_count < col_max; col_count++)
                    {
                        getline(fin, word, ',');
                        row[col_count] = stoi(word);
                        rowsInPage[pageCounter][col_count] = row[col_count];
                    }
                    pageCounter++;

                    char c;
                    c = fin.get();
                    while(c!='\n')
                    {
                        if(fin.peek()==EOF)
                        {
                            break;
                        }
                        c = fin.get();
                        
                    }
                }   

            }
            bufferManager.writePageMatrix( this->matrixName, loop_count_row, loop_count_col,  rowsInPage,  row_max, col_max);
            this->blockCount++;
            this->blockRoot = floor(sqrt(blockCount)); 
            this->rowsPerBlockCount.emplace_back(row_max);
            this->colsPerBlockCount.emplace_back(col_max);
            this->rowCount=this->matrixSize;
            this->colCount =this->matrixSize;
            pageCounter = 0;
            fin.seekg(0, ios::beg);
            int check =0;
            int traversed_Rows = 0;
            if(loop_count_col==loop_count_max-1)
            {
                if(loop_count_row==loop_count_max-1)
                    check=0;
                else
                    check=1;
            }
            for(int traverse_line=0; traverse_line<sub_matrix_size*(loop_count_row+check);traverse_line++)
            {
                char cvar=fin.get();
                while(cvar!='\n')
                {   
                    cvar = fin.get();
                    if(fin.peek()==EOF)
                        break;
                    
                }
                traversed_Rows++;
            }
        }
    }
    if (loop_count_max>0)
    {
        return true;
    }
    return false;
}

bool Matrix::sparsify()
{
    logger.log("Matrix::sparsify");
    ifstream fin(this->sourceFileName, ios::in);
    vector<pair<int, int>> VC;
    vector<int> R;
    R.push_back(0);
    int pageCount_VC=0;
    int pageCount_R=0;
    int max_ele_in_page = (BLOCK_SIZE*1024)/(sizeof(int));
    this->maxPairsPerBlockVC = max_ele_in_page/2;
    this->maxElePerBlockR = max_ele_in_page;
    int no_of_pages_VC = ceil(float(this->nz_count)/(max_ele_in_page/2)); // divide by (max_ele_in_page/2) because storing both V and C together -> (max_ele_in_page/2) pairs of int -> 1KB
    int no_of_pages_R= ceil(float(this->matrixSize+1)/max_ele_in_page); // divide by max_ele_in_page because storing R -> max_ele_in_page ints -> 1KB
    
    int row=0, col=0, nz_per_row=0, val, nz_count=0;
    string num;
    int last_r_back=0;

    for(int loop_row =0; loop_row< this->matrixSize; loop_row++)   
    {
        nz_per_row=0;

        for(int loop_col=0; loop_col < this->matrixSize-1; loop_col++)
        {
            getline(fin, num, ',');
            val = stoi(num);        
            if(val!=0)
            {
                nz_count++;
                nz_per_row++;
                pair<int, int> temp; 
                temp.first = val;
                temp.second = loop_col;
                VC.push_back(temp);
            }
            if(VC.size()==(max_ele_in_page/2))
            {  
                bufferManager.writePageSparseVC(matrixName, pageCount_VC, VC, VC.size());
                pageCount_VC++;
                this->pairsPerBlockCountVC.emplace_back(VC.size());
                VC.clear();
                
            }
        }

        getline(fin, num, '\n');
        val = stoi(num);        
        if(val!=0)
        {
            nz_count++;
            nz_per_row++;
            pair<int, int> temp; 
            temp.first = val;
            temp.second = this->matrixSize-1;
            VC.push_back(temp);
        }
        if(VC.size()==(max_ele_in_page/2))
        {
            bufferManager.writePageSparseVC(matrixName, pageCount_VC, VC, VC.size());
            pageCount_VC++;
            this->pairsPerBlockCountVC.emplace_back(VC.size());
            VC.clear();
            
            
        }

        if(!R.size())
            R.push_back(last_r_back+nz_per_row);
        else
            R.push_back(R.back()+nz_per_row);

        last_r_back = R.back();
        if(R.size()==max_ele_in_page)
        {
            bufferManager.writePageSparseR(matrixName, pageCount_R, R, R.size());
            
            pageCount_R++;
            this->elesPerBlockCountR.emplace_back(R.size());
            R.clear();
            
        }
        

    }

    if(VC.size())
    {
        bufferManager.writePageSparseVC(matrixName, pageCount_VC, VC, VC.size());
        pageCount_VC++;
        this->pairsPerBlockCountVC.emplace_back(VC.size());
        VC.clear();
        
    }
    if(R.size())
    {
        bufferManager.writePageSparseR(matrixName, pageCount_R, R, R.size());
        pageCount_R++;
        this->elesPerBlockCountR.emplace_back(R.size());
        R.clear();
        
    }
    this->rowCount=this->matrixSize;
    this->colCount =this->matrixSize;

    this->blockCountR = pageCount_R;
    this->blockCountVC = pageCount_VC;



    return true;

}



/**
 * @brief This function returns one row of the matrix using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Matrix::getNextPageMatrix(CursorMatrix *cursorMatrix)
{

    logger.log("Matrix::getNextMatrix");
    if(cursorMatrix->pagePointer == (cursorMatrix->pageMatrix).rowCount-1)
    {
        if (cursorMatrix->pageIndex_j< this->blockRoot-1)
        {
            cursorMatrix->nextPageMatrix(cursorMatrix->pageIndex_i, cursorMatrix->pageIndex_j+1, cursorMatrix->pagePointer);
        }
        else if (cursorMatrix->pageIndex_j == this->blockRoot-1 and cursorMatrix->pageIndex_i < this->blockRoot - 1)
        {
            cursorMatrix->nextPageMatrix(cursorMatrix->pageIndex_i+1, 0 ,0);
        }
    }
    else
    {
        if (cursorMatrix->pageIndex_j< this->blockRoot-1)
        {
            cursorMatrix->nextPageMatrix(cursorMatrix->pageIndex_i, cursorMatrix->pageIndex_j+1, cursorMatrix->pagePointer);
        }

        else if(cursorMatrix->pageIndex_j == this->blockRoot-1 and cursorMatrix->pageIndex_i <= this->blockRoot - 1)
        { 
            cursorMatrix->nextPageMatrix(cursorMatrix->pageIndex_i, 0 , cursorMatrix->pagePointer + 1);
        }
    }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    CursorMatrix cursorMatrix(this->matrixName, 0, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for(int colCounter=0; colCounter < this->blockRoot; colCounter++)
        {
            row = cursorMatrix.getNextMatrix();
            if(colCounter)
                fout<<",";
            this->writeRowMatrix(row, fout);
            
        }
        if(rowCounter!=this->rowCount-1)
            fout<<endl;

    }
 
    fout.close();
}

void Matrix::makePermanent_sparse()
{
    logger.log("Matrix::makePermanent_sparse");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);
    
    // if complete 0 matrix is loaded
    if(this->blockCountVC==0) // becuase no non zero elements -> blockCountVC =0
    {
        for(int i=0; i<this->matrixSize;i++)
        {
            for(int j=0;j<this->matrixSize-1; j++)
            {
                fout<<"0,";
            }
            if(i!=this->matrixSize-1)
                fout<<"0\n";
            else
                fout<<"0";
        }
        return;
    }

    CursorMatrixSparseVC cursorMatrixSparseVC(this->matrixName, 0); 
    CursorMatrixSparseR cursorMatrixSparseR(this->matrixName, 0); 
    int R_prev=0, R_curr = cursorMatrixSparseR.getNextElement();;
    vector<int> row;
    pair <int, int> VC_prev = {0,0};
    pair <int, int> VC_next = {0,0};
    int nz_printed_till_now=0, segment=0;
    for(int loop=0; loop< this->matrixSize; loop++)
    {
        R_prev=R_curr;
        R_curr = cursorMatrixSparseR.getNextElement();
        VC_prev.second =0;
        for(int x=R_prev; x<R_curr;x++)
        {
            VC_next = cursorMatrixSparseVC.getNextPair();            
            for(int i=VC_prev.second; i<VC_next.second;i++)
            {
                fout<<"0,";
            }

            if(VC_next.second!=this->matrixSize-1)
                fout<<VC_next.first<<",";
            else
                fout<<VC_next.first;
            nz_printed_till_now++;
            VC_prev = VC_next;
            VC_prev.second++;
        }
        if(loop<this->matrixSize)
        {
            for(int i=VC_prev.second; i<this->matrixSize; i++)
            {
                if(i != this->matrixSize-1)
                    fout<<"0,"; 
                else
                    fout<<"0";
            }
        }
        if(loop!=this->matrixSize-1)
            fout<<endl;
    }
 
    fout.close();
}

void Matrix::makePermanent_sparse_transpose()
{
    logger.log("Matrix::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);
    
    if(this->blockCountVC==0) // becuase no non zero elements -> BblockCountVC =0
    {
        for(int i=0; i<this->matrixSize;i++)
        {
            for(int j=0;j<this->matrixSize-1; j++)
            {
                fout<<"0,";
            }
            if(i!=this->matrixSize-1)
                fout<<"0\n";
            else
                fout<<"0";
        }
        return;
    }



    pair <int, int> VC_prev = {0,0};
    pair <int, int> VC_next = {0,0};
    int R_next =0, R_curr = 0; 
    int nz_printed_till_now=0, VC_segment_prev=0, VC_segment_next=0;

    for(int loop=0; loop < this->matrixSize; loop++)
    {
        // Resetting Cursor for VC
        CursorMatrixSparseVC cursorMatrixSparseVC(this->matrixName, 0); 
        VC_prev = {0,0};
        VC_segment_prev=0;
        VC_segment_next=0;
        for(int j=0; j < this->nz_count;j++)
        {
            VC_next = cursorMatrixSparseVC.getNextPair();
            if(VC_next.second==loop)
            {
                //Resetting cursor for R_page
                CursorMatrixSparseR cursorMatrixSparseR(this->matrixName, 0); 
                // index of VC_next.second = j
                int index_of_R=0;

                // Finding Segment
                R_next = cursorMatrixSparseR.getNextElement();
                while(R_next <= j)
                {
                    index_of_R++;
                    R_next = cursorMatrixSparseR.getNextElement();
                }
                VC_segment_next = index_of_R-1;

                //Printing in-between zeroes 
                for(int zero_count = VC_segment_prev; zero_count<VC_segment_next;zero_count++)
                    fout<<"0,";

                // Print the corresponding value after printing zeroes
                if(VC_segment_next!=this->matrixSize-1)
                    fout<<VC_next.first<<",";
                else
                    fout<<VC_next.first;
                nz_printed_till_now++;
                VC_segment_prev = VC_segment_next+1;
            }
        }

        //Printing Trailing zeroes for each row
        for(int zero_count = VC_segment_prev; zero_count<this->matrixSize; zero_count++)
        {
            if(zero_count != this->matrixSize-1)
                    fout<<"0,"; 
                else
                    fout<<"0";
        }
        if(loop!=this->matrixSize-1)
            fout<<endl;
    }
 
    fout.close();
}

bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

Cursor Matrix::getCursor()
{
    logger.log("Matrix::getCursor");
    Cursor cursor(this->matrixName, 0);
    return cursor;
}


void Matrix::print()
{
    logger.log("Matrix::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount); //this->rowCount = this->matrixSize

    CursorMatrix cursorMatrix(this->matrixName, 0, 0); 
    vector<int> row; // reads at max 16 columns and not the whole row if matrixSize > 16
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        for(int colCounter=0; colCounter < this->blockRoot; colCounter++)
        {
            row = cursorMatrix.getNextMatrix();
            if(colCounter)
                cout<<"\t";
            this->writeRowMatrixTerminal(row, cout);
            
        }
        cout<<endl;

    }
    printRowCount(this->rowCount);
}

void Matrix::print_sparse()
{
    logger.log("Matrix::print_sparse");
    uint count = fmin((long long)PRINT_COUNT, this->matrixSize);

    // Loaded a complete zero matrix (Will be always sparse)
    if(this->blockCountVC==0) // becuase no non zero elements -> BblockCountVC =0
    {
        for(int i=0; i<count;i++)
        {
            for(int j=0;j<this->matrixSize-1; j++)
            {
                cout<<"0\t";
            }
            cout<<"0\n";
        }
        printRowCount(this->rowCount);
        return;
    }

    CursorMatrixSparseVC cursorMatrixSparseVC(this->matrixName, 0); 
    CursorMatrixSparseR cursorMatrixSparseR(this->matrixName, 0); 
    int R_prev=0, R_curr = cursorMatrixSparseR.getNextElement();;
    vector<int> row;
    pair <int, int> VC_prev = {0,0};
    pair <int, int> VC_next = {0,0};
    int nz_printed_till_now=0, segment=0;
    for(int loop=0; loop< count; loop++)
    {
        R_prev=R_curr;
        R_curr = cursorMatrixSparseR.getNextElement();
        VC_prev.second =0;
        for(int x=R_prev; x<R_curr;x++)
        {
            VC_next = cursorMatrixSparseVC.getNextPair();            
            for(int i=VC_prev.second; i<VC_next.second;i++)
            {
                cout<<"0\t";
            }

            cout<<VC_next.first<<"\t";
            nz_printed_till_now++;
            VC_prev = VC_next;
            VC_prev.second++;
        }
        if(loop<count)
        {
            for(int i=VC_prev.second; i<this->matrixSize; i++)
            {
                cout<<"0\t";
            }
        }
        cout<<endl;
    }

    printRowCount(this->rowCount);

}

void Matrix::print_sparse_transpose()
{
    logger.log("Matrix::print_sparse_transpose");
    uint count = fmin((long long)PRINT_COUNT, this->matrixSize);

    // Loaded a complete zero matrix (Will be always sparse)
    if(this->blockCountVC==0) // becuase no non zero elements -> BblockCountVC =0
    {
        for(int i=0; i<count-1;i++)
        {
            for(int j=0;j<this->matrixSize-1; j++)
            {
                cout<<"0\t";
            }
            cout<<"0\n";
        }
        printRowCount(this->rowCount);
        return;
    }

    pair <int, int> VC_prev = {0,0};
    pair <int, int> VC_next = {0,0};
    int R_next =0, R_curr = 0; 
    int nz_printed_till_now=0, VC_segment_prev=0, VC_segment_next=0;

    for(int loop=0; loop < count; loop++)
    {
        //Resetting cursor for VC_page
        CursorMatrixSparseVC cursorMatrixSparseVC(this->matrixName, 0); 
        VC_prev = {0,0};
        VC_segment_prev=0;
        VC_segment_next=0;
        for(int j=0; j < this->nz_count;j++)
        {
            VC_next = cursorMatrixSparseVC.getNextPair();
            if(VC_next.second==loop)
            {
                //Finding segment 
                //Resetting cursor for R_page
                CursorMatrixSparseR cursorMatrixSparseR(this->matrixName, 0); 
                // index of VC_next.second = j
                int index_of_R=0;
                R_next = cursorMatrixSparseR.getNextElement();
                while(R_next <= j)
                {
                    index_of_R++;
                    R_next = cursorMatrixSparseR.getNextElement();
                }
                VC_segment_next = index_of_R-1;

                //Printing in-between zeroes 
                for(int zero_count = VC_segment_prev; zero_count<VC_segment_next;zero_count++)
                    cout<<"0\t";

                // Print the corresponding value after printing zeroes
                cout<<VC_next.first<<"\t";
                nz_printed_till_now++;
                VC_segment_prev = VC_segment_next+1;
            }
        }

        //Printing Trailing zeroes for each row
        for(int zero_count = VC_segment_prev; zero_count<this->matrixSize; zero_count++)
            cout<<"0\t";
        cout<<endl;
    }

    printRowCount(this->rowCount);
}


void Matrix::transpose_entry()
{
    logger.log("Matrix::transpose_entry");
    if(this->isSparse)
    {
        this->isTranspose= (!this->isTranspose);
        return;
    }
    int blockroot = this->blockRoot;

    
    for(int pageIndex_i=0; pageIndex_i< blockroot; pageIndex_i++)
    {
        for(int pageIndex_j=pageIndex_i; pageIndex_j<this->blockRoot; pageIndex_j++)
        {
            if(pageIndex_j!=pageIndex_i)
            {
                CursorMatrix cursorMatrix_ij(this->matrixName, pageIndex_i, pageIndex_j); 
                vector<vector<int>> B_ij = cursorMatrix_ij.pageMatrix.transpose();
                int rowCount_ij = cursorMatrix_ij.pageMatrix.columnCount;
                int columnCount_ij = cursorMatrix_ij.pageMatrix.rowCount;

                CursorMatrix cursorMatrix_ji(this->matrixName, pageIndex_j, pageIndex_i); 
                vector<vector<int>> B_ji = cursorMatrix_ji.pageMatrix.transpose();
                int rowCount_ji = cursorMatrix_ji.pageMatrix.columnCount;
                int columnCount_ji = cursorMatrix_ji.pageMatrix.rowCount;

                string matrixName = cursorMatrix_ij.pageMatrix.matrixName;
                int pageI_i = cursorMatrix_ij.pageMatrix.pageIndex_i;
                int pageI_j = cursorMatrix_ij.pageMatrix.pageIndex_j;
                bufferManager.writePageMatrix(matrixName, pageI_j, pageI_i, B_ij, rowCount_ij, columnCount_ij);
                bufferManager.writePageMatrix(matrixName, pageI_i, pageI_j, B_ji, rowCount_ji, columnCount_ji);
            }
            else
            {
                CursorMatrix cursorMatrix_ij(this->matrixName, pageIndex_i, pageIndex_j); 
                vector<vector<int>> B_ij = cursorMatrix_ij.pageMatrix.transpose();
                int rowCount_ij = cursorMatrix_ij.pageMatrix.columnCount;
                int columnCount_ij = cursorMatrix_ij.pageMatrix.rowCount;
                string matrixName = cursorMatrix_ij.pageMatrix.matrixName;
                int pageI_i = cursorMatrix_ij.pageMatrix.pageIndex_i;
                int pageI_j = cursorMatrix_ij.pageMatrix.pageIndex_j;
                
                bufferManager.writePageMatrix(matrixName, pageI_j, pageI_i, B_ij, rowCount_ij, columnCount_ij);            
            }

        }
    }

}


// ######################################################################## SPARSE VC BEGINS HERE

void Matrix::getNextPageSparseVC(CursorMatrixSparseVC *cursorMatrixSparseVC)
{
    logger.log("Matrix::getNextPageVC");

        if (cursorMatrixSparseVC->pageIndex < this->blockCountVC - 1)
        {
            cursorMatrixSparseVC->nextPageVC(cursorMatrixSparseVC->pageIndex+1);
        }
}


// ######################################################################## SPARSE R BEGINS HERE
void Matrix::getNextPageSparseR(CursorMatrixSparseR *cursorMatrixSparseR)
{
    logger.log("Matrix::getNextPageR");

        if (cursorMatrixSparseR->pageIndex < this->blockCountR - 1)
        {
            cursorMatrixSparseR->nextPageR(cursorMatrixSparseR->pageIndex+1);
        }
}
