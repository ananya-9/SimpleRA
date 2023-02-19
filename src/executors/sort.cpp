#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 * 
 * SYNTAX : <new_table_name> <- SORT <table_name> BY <column_name> IN ASC | DESC BUFFER <buffer_size>
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    if(tokenizedQuery.size()!= 8 || tokenizedQuery[4] != "BY" || tokenizedQuery[6] != "IN"){
        if(tokenizedQuery.size() != 10)
        {
            cout<<"SYNTAX ERROR"<<endl;
            return false;
        }
    }

    if(tokenizedQuery.size() == 10 and tokenizedQuery[8] != "BUFFER")
    {
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }

    parsedQuery.queryType = SORT;
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortRelationName = tokenizedQuery[3];
    parsedQuery.sortColumnName = tokenizedQuery[5];
    string sortingStrategy = tokenizedQuery[7];
    if(sortingStrategy == "ASC")
        parsedQuery.sortingStrategy = ASC;
    else if(sortingStrategy == "DESC")
        parsedQuery.sortingStrategy = DESC;
    else{
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }

    if(tokenizedQuery.size() == 10)
        parsedQuery.sortBufferSize = stoi(tokenizedQuery[9]);

    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(tableCatalogue.isTable(parsedQuery.sortResultRelationName)){
        cout<<"SEMANTIC ERROR: Resultant relation already exists"<<endl;
        return false;
    }

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    if(!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnName, parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
        return false;
    }

    return true;
}

void executeSORT(){

    BLOCK_ACCESS=0;
    int initial_block_cnt = BLOCK_COUNT;
    BLOCK_COUNT = parsedQuery.sortBufferSize;
    int sortingStrategy;
    if(parsedQuery.sortingStrategy == ASC)
        sortingStrategy = 1; 
    else if(parsedQuery.sortingStrategy == DESC)
        sortingStrategy = -1;
    logger.log("executeSORT");
    
    Table table = *(tableCatalogue.getTable(parsedQuery.sortRelationName));
    int sortColIndex = table.getColumnIndex(parsedQuery.sortColumnName);
    table.chunkify(sortColIndex);

    vector<Cursor> vector_cursor; 

    Table *resultantTable = new Table(parsedQuery.sortResultRelationName, table.columns);

    for(int i=0; i<table.chunkCount; i++)
    {
        string chunk_name = parsedQuery.sortRelationName + "_chunk_" + to_string(i);
        Table chunk_table = *(tableCatalogue.getTable(chunk_name));
        Cursor chunk_cursor = chunk_table.getCursor();
        vector_cursor.emplace_back(chunk_cursor);
    }

    vector<vector<int>> chunk_rows; 
    int rows_written = 0;
    for(int i=0; i<table.chunkCount; i++)
    {
        vector<int> row;
        row = vector_cursor[i].getNext();
        chunk_rows.emplace_back(row);
    }

    while(rows_written < table.rowCount)
    {
        int index=0; 
        int minimum = INT_MAX;
        int maximum = INT_MIN;
        
        for(int i=0; i<chunk_rows.size(); i++)
        {
            if(sortingStrategy == 1)
            {
                if(chunk_rows[i][sortColIndex] < minimum)
                {
                    minimum = chunk_rows[i][sortColIndex];
                    index = i;
                }
            }

            else if(sortingStrategy == -1)
            {
                if(chunk_rows[i][sortColIndex] > maximum)
                {
                    maximum = chunk_rows[i][sortColIndex];
                    index = i;
                }
            }
        }

        resultantTable->writeRow(chunk_rows[index]);
        rows_written++;
        vector<int> new_row = vector_cursor[index].getNext();

        if(!new_row.empty())
            chunk_rows[index] = new_row;
        else
        {
            auto cursor_iterator= vector_cursor.begin()+index;
            vector_cursor.erase(cursor_iterator);

            auto chunk_rows_iterator= chunk_rows.begin()+index;
            chunk_rows.erase(chunk_rows_iterator);
        }
    }
    resultantTable->blockify();
    tableCatalogue.insertTable(resultantTable);
    BLOCK_COUNT = initial_block_cnt;
    cout<<endl;
    cout<<"BLOCK ACCESSES : "<<BLOCK_ACCESS<<endl;

    for(int i=0; i<table.chunkCount;i++)
    {
        string chunk_name = parsedQuery.sortRelationName + "_chunk_" + to_string(i);
        tableCatalogue.deleteTable(chunk_name);
    }

    return;
}
