/*
***********************************************************************
  DEADASSIGN.C : IMPLEMENT THE DEAD CODE ELIMINATION OPTIMIZATION HERE
************************************************************************
*/

#include "deadassign.h"

int change;
refVar *last, *head;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE HEAD AND LAST POINTERS FOR THE REFERENCED 
  VARIABLE LIST.
************************************************************************
*/

void init()
{ 
    head = NULL;
    last = head;
}

/*
***********************************************************************
  FUNCTION TO FREE THE REFERENCED VARIABLE LIST
************************************************************************
*/

void FreeList()
{
   refVar* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO IDENTIFY IF A VARIABLE'S REFERENCE IS ALREADY TRACKED
************************************************************************
*/
bool VarExists(char* name) {
   refVar *node;
   node = head;
   while (node != NULL) {
       if (!strcmp(name, node->name)) {
           return true;
       }
       node = node->next;
    }
    return false;
}

/*
***********************************************************************
  FUNCTION TO ADD A REFERENCE TO THE REFERENCE LIST
************************************************************************
*/
void UpdateRefVarList(char* name) {
    refVar* node = malloc(sizeof(refVar));
    if (node == NULL) return;
    node->name = name;
    node->next = NULL;
    if (head == NULL) {
        last = node;
        head = node;
    }
    else {
        last->next = node;
        last = node;
    }
}

/*
****************************************************************************
  FUNCTION TO PRINT OUT THE LIST TO SEE ALL VARIABLES THAT ARE USED/REFERRED
  AFTER THEIR ASSIGNMENT. YOU CAN USE THIS FOR DEBUGGING PURPOSES OR TO CHECK
  IF YOUR LIST IS GETTING UPDATED CORRECTLY
******************************************************************************
*/
void PrintRefVarList() {
    refVar *node;
    node = head;
    if (node == NULL) {
        printf("\nList is empty"); 
        return;
    }
    while (node != NULL) {
        printf("\t %s", node->name);
        node = node->next;
    }
}

/*
***********************************************************************
  FUNCTION TO UPDATE THE REFERENCE LIST WHEN A VARIABLE IS REFERENCED 
  IF NOT DONE SO ALREADY.
************************************************************************
*/
void UpdateRef(Node* node) {
      if (node->right != NULL && node->right->exprCode == VARIABLE) {
          if (!VarExists(node->right->name)) {
              UpdateRefVarList(node->right->name);
          }
      }
      if (node->left != NULL && node->left->exprCode == VARIABLE) {
          if (!VarExists(node->left->name)) {
              UpdateRefVarList(node->left->name);
          }
      }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
********************************************************************
  THIS FUNCTION IS MEANT TO TRACK THE REFERENCES OF EACH VARIABLE
  TO HELP DETERMINE IF IT WAS USED OR NOT LATER
********************************************************************
*/

void TrackRef(Node* funcNode) {
     NodeList* statements = funcNode->statements;
     Node *node;
     init();
     while (statements != NULL) {
        node = statements->node;
        Node* right_node;

        // Check return statement
        if (node->stmtCode == RETURN) {
            right_node = node->left;
        }
        else if (node->stmtCode == ASSIGN) {
            right_node = node->right;
        }
        // handle different variable referencing scenarios
        if (right_node->exprCode == VARIABLE) {
            UpdateRef(node);
        }
        // handle diff OPERATIONS
        else if (right_node->exprCode == OPERATION) {
            // FUNCTIONCALL
            if (right_node->opCode == FUNCTIONCALL) {
                NodeList* args = right_node->arguments;
                while (args != NULL) {
                    Node* temp = args->node;
                    if (temp->exprCode == VARIABLE) {
                        UpdateRefVarList(temp->name);
                    }
                    args = args->next;
                }
            }
            // UNARY operation
            else if (right_node->opCode == NEGATE) {
                Node* var1 = right_node->left;
                if (var1->exprCode == VARIABLE) {
                    UpdateRef(right_node);
                }
            }
            // BINARY operations
            else {
                Node *var1= right_node->right;
                Node *var2= right_node->left;
                if (var1->exprCode == VARIABLE){
                    UpdateRef(right_node);
                }
                if (var2->exprCode == VARIABLE){
                    UpdateRef(right_node);
                }
            }
        }
        statements = statements->next;      
     }
}


/*
***************************************************************
  THIS FUNCTION IS MEANT TO DO THE ACTUAL DEADCODE REMOVAL
  BASED ON THE INFORMATION OF TRACKED REFERENCES
****************************************************************
*/
NodeList* RemoveDead(NodeList* statements) {
    NodeList *prev = NULL, *tmp, *first;
    first = statements;

    while (statements != NULL) {
        if (statements->node->type == STATEMENT &&
            statements->node->stmtCode == ASSIGN &&
            !VarExists(statements->node->name)) {

            change = 1;
            // Remove the dead assignment
            tmp = statements;
            if (prev == NULL) {
                first = statements->next;
            } else {
                prev->next = statements->next;
            }
            statements = statements->next;
            free(tmp);
        } else {
            prev = statements;
            statements = statements->next;
        }
    }

    return first;
}


/*
********************************************************************
  THIS FUNCTION SHOULD ENSURE THAT THE DEAD CODE REMOVAL PROCESS
  OCCURS CORRECTLY FOR ALL THE FUNCTIONS IN THE PROGRAM
********************************************************************
*/
bool DeadAssign(NodeList* funcdecls) {
    bool madeChange = false;
    // Reset change variable before processing the functions
    change = 0;
    while (funcdecls != NULL) {
        if (funcdecls->node->type == FUNCTIONDECL) {
            init();
            TrackRef(funcdecls->node);
            NodeList* updatedStatements = RemoveDead(funcdecls->node->statements);

            if (updatedStatements != funcdecls->node->statements) {
                funcdecls->node->statements = updatedStatements;
            }

            FreeList();
        }
        funcdecls = funcdecls->next;
    }
    // If change was made in any of the function's statements, update the madeChange flag
    if (change == 1) madeChange = true;
    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
 