/*
   CONSTFOLDING.C : THIS FILE IMPLEMENTS THE CONSTANT FOLDING OPTIMIZATION
*/

#include "constfolding.h"
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL 
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO 
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/                                                                                                          
bool madeChange;

/*
******************************************************************************************
FUNCTION TO CALCULATE THE CONSTANT EXPRESSION VALUE 
OBSERVE THAT THIS IMPLEMENTATION CONSIDERS ADDITIONAL OPTIMIZATIONS SUCH AS:
1.  IDENTITY MULTIPLY = 1 * ANY_VALUE = ANY_VALUE - AVOID MULTIPLICATION CYCLE IN THIS CASE
2.  ZERO MULTIPLY = 0 * ANY_VALUE = 0 - AVOID MULTIPLICATION CYCLE
3.  DIVIDE BY ONE = ORIGINAL_VALUE - AVOID DIVISION CYCLE
4.  SUBTRACT BY ZERO = ORIGINAL_VALUE - AVOID SUBTRACTION
5.  MULTIPLICATION BY 2 = ADDITION BY SAME VALUE [STRENGTH REDUCTION]
******************************************************************************************
*/
long CalcExprValue(Node* node)
{
     long result;
     Node *leftNode, *rightNode;
     leftNode = node->left;
     rightNode = node->right; 
     switch(node->opCode){
         case MULTIPLY:
             if (leftNode->value == 1) {
                 result = rightNode->value;
             } 
             else if (rightNode->value == 1) {
                 result = leftNode->value;
             }
             else if (leftNode->value == 0 || rightNode->value == 0) {
                 result = 0;
             }
             else if (leftNode->value == 2) {
                 result = rightNode->value + rightNode->value;
             }              
             else if (rightNode->value == 2) {
                 result = leftNode->value + leftNode->value;
             }
             else {
                 result = leftNode->value * rightNode->value;
             }
             break;
         case DIVIDE:
             if (rightNode->value == 1) {
                 result = leftNode->value;
             }
             else {
                 result = leftNode->value / rightNode->value;
             }
             break;
         case ADD:
             result = leftNode->value + rightNode->value;
             break;
         case SUBTRACT:
             result = leftNode->value - rightNode->value;
             break;
         case NEGATE:
             result = -leftNode->value;
             break;
         default:
             break;
     }
     return result;
}


/*
**********************************************************************************************************************************
// YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/

/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO PROCESS THE CANDIDATE STATEMENTS AND PERFORM CONSTANT FOLDING 
WHEREVER APPLICABLE.
******************************************************************************************************
*/
long ConstFoldPerStatement(Node* stmtNodeRight) {
    long result;
    // Handle unary NEGATE operation
    if (stmtNodeRight->opCode == NEGATE && stmtNodeRight->left && stmtNodeRight->left->exprCode == CONSTANT) {
        result = CalcExprValue(stmtNodeRight);
    } 
    // Handle binary operations excluding FUNCTIONCALL
    else if (stmtNodeRight->opCode != FUNCTIONCALL && stmtNodeRight->left && stmtNodeRight->left->exprCode == CONSTANT && stmtNodeRight->right && stmtNodeRight->right->exprCode == CONSTANT) {
        result = CalcExprValue(stmtNodeRight);
    } else {
        return -1;
    }

    return result;
}
                                                               
/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO IDENTIFY THE STATEMENTS THAT ARE ACTUAL CANDIDATES FOR CONSTANT FOLDING
AND CALL THE APPROPRIATE FUNCTION FOR THE IDENTIFIED CANDIDATE'S CONSTANT FOLDING
******************************************************************************************************
*/
void ConstFoldPerFunction(Node* funcNode) {
    Node *rightNode, *leftNode, *stmtNodeRight;
    long result;
    NodeList* statements = funcNode->statements;

    while (statements) {
        Node* statement = statements->node;
        if (!statement) {
            statements = statements->next;
            continue;
        }
        // Determine the current expression based on the statement type
        if (statement->stmtCode == ASSIGN) {
            rightNode = statement->right;
        } else if (statement->stmtCode == RETURN) {
            rightNode = statement->left;
        } else {
            rightNode = NULL;
        }
        // Process potential foldable operations excluding FUNCTIONCALL
        if (rightNode && rightNode->exprCode == OPERATION && rightNode->opCode != FUNCTIONCALL) {
            result = ConstFoldPerStatement(rightNode);
            // If folding was successful, update the statement and perform cleanup
            if (result != -1) {
                Node* newNumberNode = CreateNumber(result);
                madeChange = true;
                if (statement->stmtCode == ASSIGN) {
                    statement->right = newNumberNode;
                } else if (statement->stmtCode == RETURN) {
                    statement->left = newNumberNode;
                }
                // free old nodes
                if (rightNode->opCode == NEGATE) {
                    FreeUnaryOperation(rightNode);
                } else {
                    FreeBinaryOperation(rightNode); // avoid leaks
                }
            }
        }
        statements = statements->next; 
    }
}

/*
**********************************************************************************************************************************
// YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
********************************************************************************************************************************
*/

/*
*****************************************************************************************************
THIS FUNCTION ENSURES THAT THE CONSTANT FOLDING OPTIMIZATION IS DONE FOR EVERY FUNCTION IN THE PROGRAM
******************************************************************************************************
*/

bool ConstantFolding(NodeList* list) {
    madeChange = false;
    while (list != NULL) {
        // Attempt constant folding on the current function node
        ConstFoldPerFunction(list->node);
	    list = list->next;
    }
    return madeChange;
}

/*
****************************************************************************************************************************
 END OF CONSTANT FOLDING
*****************************************************************************************************************************
*/                
