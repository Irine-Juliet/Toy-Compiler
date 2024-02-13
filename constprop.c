/*
********************************************************************************
  CONSTPROP.C : IMPLEMENT THE DOWNSTREAM CONSTANT PROPOGATION OPTIMIZATION HERE
*********************************************************************************
*/

#include "constprop.h"

refConst *lastNode, *headNode;
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO FREE THE CONSTANTS-ASSOCIATED VARIABLES LIST
************************************************************************
*/
void FreeConstList()
{
   refConst* tmp;
   while (headNode != NULL)
    {
       tmp = headNode;
       headNode = headNode->next;
       free(tmp);
    }

}

/*
*************************************************************************
  FUNCTION TO ADD A CONSTANT VALUE AND THE ASSOCIATED VARIABLE TO THE LIST
**************************************************************************
*/
void UpdateConstList(char* name, long val) {
    refConst* node = malloc(sizeof(refConst));
    if (node == NULL) return;
    node->name = name;
    node->val = val;
    node->next = NULL;
    if (headNode == NULL) {
        lastNode = node;
        headNode = node;
    }
    else {
        lastNode->next = node;
        lastNode = node;
    }
}

/*
*****************************************************************************
  FUNCTION TO LOOKUP IF A CONSTANT ASSOCIATED VARIABLE IS ALREADY IN THE LIST
******************************************************************************
*/
refConst* LookupConstList(char* name) {
    refConst *node;
    node = headNode; 
    while (node != NULL){
        if (!strcmp(name, node->name))
            return node;
        node = node->next;
    }
    return NULL;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
************************************************************************************
  THIS FUNCTION IS MEANT TO UPDATE THE CONSTANT LIST WITH THE ASSOCIATED VARIABLE
  AND CONSTANT VALUE WHEN ONE IS SEEN. IT SHOULD ALSO PROPOGATE THE CONSTANTS WHEN 
  WHEN APPLICABLE. YOU CAN ADD A NEW FUNCTION IF YOU WISH TO MODULARIZE BETTER.
*************************************************************************************
*/
void TrackConst(NodeList* statements) {
    Node* node;
    while (statements != NULL) {
        node = statements->node; 
        // check assignment
        if (node->stmtCode == ASSIGN) {
            if (statements->node->right->exprCode == CONSTANT){
                UpdateConstList(statements->node->name, statements->node->right->value);
            }
        }
        statements = statements->next;
    }
}

bool ConstProp(NodeList* funcdecls) {
    bool madeChange = false;

    while (funcdecls != NULL) {
        Node* node = funcdecls->node;
        NodeList* statements = node->statements;
        TrackConst(statements);
        while (statements != NULL) {
            Node* statement = statements->node;

            if (statement->stmtCode == ASSIGN) {
                madeChange |= propagateConstantsInAssignment(statement);
            } else if (statement->stmtCode == RETURN) {
                madeChange |= propagateConstantsInReturn(statement);
            }
            statements = statements->next;
        }
        FreeConstList();
        funcdecls = funcdecls->next;
    }
    return madeChange;
}

bool propagateConstantsInAssignment(Node* statement) {
    bool madeChange = false;
    Node* variableName = statement->right;
    if (variableName->exprCode == VARIABLE) {
        refConst* res = LookupConstList(variableName->name);
        if (res != NULL) {
            statement->right = CreateNumber(res->val);
            FreeVariable(variableName);
            madeChange = true;
        }
    } else if (variableName->exprCode == OPERATION) {
        madeChange |= propagateConstantsInOperation(variableName);
    }
    return madeChange;
}

bool propagateConstantsInReturn(Node* returnStatement) {
    bool madeChange = false;

    Node* return_val = return_val->left;
    if (return_val->exprCode == VARIABLE) {
        refConst* res = LookupConstList(return_val->name);
        if (res != NULL) {
            returnStatement->left = CreateNumber(res->val);
            FreeVariable(return_val);
            madeChange = true;
        }
    }
    return madeChange;
}

bool propagateConstantsInOperation(Node* operationNode) {
    bool madeChange = false;
    
    // Handle FUNCTIONCALL
    if (operationNode->opCode == FUNCTIONCALL) {
        NodeList* temp = operationNode->arguments;
        while (temp != NULL) {
            Node* param =  temp->node;
            
            if (param->exprCode == PARAMETER || param->exprCode == VARIABLE) {
                refConst* res = LookupConstList(param->name);
                if (res != NULL) {
                    Node* param1 = param;
                    temp->node = CreateNumber(res->val);
                    FreeParameter(param1);
                    madeChange = true;
                }
            }
            
            temp = temp->next;
        }
    }
    // Handle UNARY OPERATION: NEGATE
    else if (operationNode->opCode == NEGATE) {
        Node* left_operand = operationNode->left;
        if (left_operand->exprCode == VARIABLE) {
            refConst* res = LookupConstList(left_operand->name);
            if (res != NULL) {
                operationNode->left = CreateNumber(res->val);
                FreeVariable(left_operand);
                madeChange = true;
            }
        }
    }
    // Handle BINARY OPERATIONS
    else {
        // left operand
        Node* left_operand = operationNode->left;
        if (left_operand && left_operand->exprCode == VARIABLE) {
            refConst* res = LookupConstList(left_operand->name);
            if (res != NULL) {
                operationNode->left = CreateNumber(res->val);
                FreeVariable(left_operand);
                madeChange = true;
            }
        }
        
        // right operand
        Node* right_operand = operationNode->right;
        if (right_operand && right_operand->exprCode == VARIABLE) {
            refConst* res = LookupConstList(right_operand->name);
            if (res != NULL) {
                operationNode->right = CreateNumber(res->val);
                FreeVariable(right_operand);
                madeChange = true;
            }
        }
    }
    return madeChange;
}
/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
