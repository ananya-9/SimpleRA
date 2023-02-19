#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINTMATRIX relation_name
 */
bool syntacticParsePRINTMATRIX()
{
    logger.log("syntacticParsePRINTMATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINTMATRIX;
    parsedQuery.printRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParsePRINTMATRIX()
{
    logger.log("semanticParsePRINTMATRIX");
    if (!matrixCatalogue.isMatrix(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINTMATRIX()
{
    logger.log("executePRINTMATRIX");
    Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
    if(matrix->isSparse)
    {
        if(matrix->isTranspose)
            matrix->print_sparse_transpose();
        else
            matrix->print_sparse();
    }
    else
        matrix->print();
    return;
}
