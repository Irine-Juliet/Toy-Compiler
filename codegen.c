/*
***********************************************************************
  CODEGEN.C : IMPLEMENT CODE GENERATION HERE
************************************************************************
*/
#include "codegen.h"

int argCounter;
int lastUsedOffset;
char lastOffsetUsed[100];
FILE *fptr;
regInfo *regList, *regHead, *regLast;
varStoreInfo *varList, *varHead, *varLast;
char* registers[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9", "%rax",};

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE THE ASSEMBLY FILE WITH FUNCTION DETAILS
************************************************************************
*/
void InitAsm(char* funcName) {
    fprintf(fptr, "\n.globl %s", funcName);
    fprintf(fptr, "\n%s:", funcName); 

    // Init stack and base ptr
    fprintf(fptr, "\npushq %%rbp");  
    fprintf(fptr, "\nmovq %%rsp, %%rbp"); 
}

/*
***************************************************************************
   FUNCTION TO WRITE THE RETURNING CODE OF A FUNCTION IN THE ASSEMBLY FILE
****************************************************************************
*/
void RetAsm() {
    fprintf(fptr,"\npopq  %%rbp");
    fprintf(fptr, "\nretq\n");
} 

/*
***************************************************************************
  FUNCTION TO CONVERT OFFSET FROM LONG TO CHAR STRING 
****************************************************************************
*/
void LongToCharOffset() {
     lastUsedOffset = lastUsedOffset - 8;
     snprintf(lastOffsetUsed, 100,"%d", lastUsedOffset);
     strcat(lastOffsetUsed,"(%rbp)");
}

/*
***************************************************************************
  FUNCTION TO CONVERT CONSTANT VALUE TO CHAR STRING
****************************************************************************
*/
void ProcessConstant(Node* opNode) {
     char value[10];
     LongToCharOffset();
     snprintf(value, 10,"%ld", opNode->value);
     char str[100];
     snprintf(str, 100,"%d", lastUsedOffset);
     strcat(str,"(%rbp)");
     AddVarInfo("", str, opNode->value, true);
     fprintf(fptr, "\nmovq  $%s, %s", value, str);
}

/*
***************************************************************************
  FUNCTION TO SAVE VALUE IN ACCUMULATOR (RAX)
****************************************************************************
*/
void SaveValInRax(char* name) {
    char *tempReg;
    tempReg = GetNextAvailReg(true);
    if (!(strcmp(tempReg, "NoReg"))) {
        LongToCharOffset();
        fprintf(fptr, "\n movq %%rax, %s", lastOffsetUsed);
        UpdateVarInfo(name, lastOffsetUsed, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
    else {
        fprintf(fptr, "\nmovq %%rax, %s", tempReg);
        UpdateRegInfo(tempReg, 0);
        UpdateVarInfo(name, tempReg, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
}



/*
***********************************************************************
  FUNCTION TO ADD VARIABLE INFORMATION TO THE VARIABLE INFO LIST
************************************************************************
*/
void AddVarInfo(char* varName, char* location, long val, bool isConst) {
   varStoreInfo* node = malloc(sizeof(varStoreInfo));
   node->varName = varName;
   node->value = val;
   strcpy(node->location,location);
   node->isConst = isConst;
   node->next = NULL;
   node->prev = varLast;
   if (varHead == NULL) {
       varHead = node;
       varLast = node;;
       varList = node;
   } else {
       //node->prev = varLast;
       varLast->next = node;
       varLast = varLast->next;
   }
   varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO FREE THE VARIABLE INFORMATION LIST
************************************************************************
*/
void FreeVarList()
{  
   varStoreInfo* tmp;
   while (varHead != NULL)
    {  
       tmp = varHead;
       varHead = varHead->next;
       free(tmp);
    }
   varLast = NULL;
}

/*
***********************************************************************
  FUNCTION TO LOOKUP VARIABLE INFORMATION FROM THE VARINFO LIST
************************************************************************
*/
char* LookUpVarInfo(char* name, long val) {
    varList = varLast;
    if (varList == NULL) printf("NULL varlist");
    while (varList != NULL) {
        if (varList->isConst == true) {
            if (varList->value == val) return varList->location;
        }
        else {
            if (!strcmp(name,varList->varName)) return varList->location;
        }
        varList = varList->prev;
    }
    varList = varHead;
    return "";
}

/*
***********************************************************************
  FUNCTION TO UPDATE VARIABLE INFORMATION 
************************************************************************
*/
void UpdateVarInfo(char* varName, char* location, long val, bool isConst) {
  
   if (!(strcmp(LookUpVarInfo(varName, val), ""))) {
       AddVarInfo(varName, location, val, isConst);
   }
   else {
       varList = varHead;
       if (varList == NULL) printf("NULL varlist");
       while (varList != NULL) {
           if (!strcmp(varList->varName,varName)) {
               varList->value = val;
               strcpy(varList->location,location);
               varList->isConst = isConst;
               break;
        }
        varList = varList->next;
       }
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE VARIABLE INFORMATION LIST
************************************************************************
*/
void PrintVarListInfo() {
    varList = varHead;
    if (varList == NULL) printf("NULL varlist");
    while (varList != NULL) {
        if (!varList->isConst) {
            printf("\t %s : %s", varList->varName, varList->location);
        }
        else {
            printf("\t %ld : %s", varList->value, varList->location);
        }
        varList = varList->next;
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO ADD NEW REGISTER INFORMATION TO THE REGISTER INFO LIST
************************************************************************
*/
void AddRegInfo(char* name, int avail) {

   regInfo* node = malloc(sizeof(regInfo));
   node->regName = name;
   node->avail = avail;
   node->next = NULL; 

   if (regHead == NULL) {
       regHead = node;
       regList = node;
       regLast = node;
   } else {
       regLast->next = node;
       regLast = node;
   }
   regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO FREE REGISTER INFORMATION LIST
************************************************************************
*/
void FreeRegList()
{  
   regInfo* tmp;
   while (regHead != NULL)
    {  
       tmp = regHead;
       regHead = regHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO UPDATE THE AVAILIBILITY OF REGISTERS IN THE REG INFO LIST
************************************************************************
*/
void UpdateRegInfo(char* regName, int avail) {
    while (regList != NULL) {
        if (regName == regList->regName) {
            regList->avail = avail;
        }
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO RETURN THE NEXT AVAILABLE REGISTER
************************************************************************
*/
char* GetNextAvailReg(bool noAcc) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (regList->avail == 1) {
            if (!noAcc) return regList->regName;
            // if not rax and dont return accumulator set to true, return the other reg
            // if rax and noAcc == true, skip to next avail
            if (noAcc && strcmp(regList->regName, "%rax")) { 
                return regList->regName;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return "NoReg";
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF ANY REGISTER APART FROM OR INCLUDING 
  THE ACCUMULATOR(RAX) IS AVAILABLE
************************************************************************
*/
int IfAvailReg(bool noAcc) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (regList->avail == 1) {
            // registers available
            if (!noAcc) return 1;
            if (noAcc && strcmp(regList->regName, "%rax")) {
                return 1;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return 0;
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF A SPECIFIC REGISTER IS AVAILABLE
************************************************************************
*/
bool IsAvailReg(char* name) {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        if (!strcmp(regList->regName, name)) {
           if (regList->avail == 1) {
               return true;
           } 
        }
        regList = regList->next;
    }
    regList = regHead;
    return false;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE REGISTER INFORMATION
************************************************************************
*/
void PrintRegListInfo() {
    regList = regHead;
    if (regList == NULL) printf("NULL reglist");
    while (regList != NULL) {
        printf("\t %s : %d", regList->regName, regList->avail);
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO CREATE THE REGISTER LIST
************************************************************************
*/
void CreateRegList() {
    // Create the initial reglist which can be used to store variables.
    // 4 general purpose registers : AX, BX, CX, DX
    // 4 special purpose : SP, BP, SI , DI. 
    // Other registers: r8, r9
    // You need to decide which registers you will add in the register list 
    // use. Can you use all of the above registers?
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ***************************************
    */ 
    AddRegInfo("%rdi", 1);
    AddRegInfo("%rsi", 1);
    AddRegInfo("%rdx", 1);
    AddRegInfo("%rcx", 1);
    //AddRegInfo("%rbx", 1);
    // add more registers
    AddRegInfo("%r8", 1);
    AddRegInfo("%r9", 1);
    AddRegInfo("%rax", 1);
}



/*
***********************************************************************
  THIS FUNCTION IS MEANT TO PUT THE FUNCTION ARGUMENTS ON STACK
************************************************************************
*/
int PushArgOnStack(NodeList* arguments) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    int arg_count = 0;
    NodeList* arg_iterator = arguments;
    while (arg_iterator != NULL) 
    {
        arg_iterator = arg_iterator->next;
        arg_count++;
    }
    // Calculate stack space for just the arguments
    int stackSpaceRequired = arg_count* 8; // Each argument takes up 8 bytes
    fprintf(fptr, "\nsubq $%d, %%rsp", stackSpaceRequired);
    
    // Process arguments: either move to registers or push to the stack
    int regIndex = 0;
    while (arguments != NULL) {
    /*
     ***********************************************************************
              TODO : YOUR CODE HERE
      THINK ABOUT WHERE EACH ARGUMENT COMES FROM. EXAMPLE WHERE IS THE 
      FIRST ARGUMENT OF A FUNCTION STORED.
     ************************************************************************
     */ 
        Node* current_arg = arguments->node;
        char* argumentName = current_arg->name;

        // get the stack offset for this argument
        LongToCharOffset();
        // Update the variable list with this argument's location on the stack
        AddVarInfo(argumentName, lastOffsetUsed, INVAL, false);

        char* reg = registers[regIndex];
        fprintf(fptr, "\nmovq %s, %s", reg, lastOffsetUsed);
        // Mark the current register as used
        UpdateRegInfo(reg, 1);
        regIndex++;
        arguments = arguments->next; 
    }
    return arg_count;
}


/*
*************************************************************************
  THIS FUNCTION IS MEANT TO GET THE FUNCTION ARGUMENTS FROM THE  STACK
**************************************************************************
*/
void PopArgFromStack(NodeList* arguments) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    int regIndex = 0; 
    
    while (arguments != NULL) {
    /*
     ***********************************************************************
              TODO : YOUR CODE HERE
      THINK ABOUT WHERE EACH ARGUMENT COMES FROM. EXAMPLE WHERE IS THE
      FIRST ARGUMENT OF A FUNCTION STORED AND WHERE SHOULD IT BE EXTRACTED
      FROM AND STORED TO..
     ************************************************************************
     */
        Node* argument = arguments->node;
        if (argument->exprCode == CONSTANT) {
            fprintf(fptr, "\nmovq $%li, %s", argument->value, registers[regIndex]);
        } else {
            if (strcmp(LookUpVarInfoByName(argument->name), "") != 0) { // returned location is not empty
                fprintf(fptr, "\nmovq %s, %s", LookUpVarInfoByName(argument->name), registers[regIndex]);
            } 
        }
        // register is now available
        UpdateRegInfo(registers[regIndex], 0); 
        regIndex++;
        arguments = arguments->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO PROCESS EACH CODE STATEMENT AND GENERATE 
  ASSEMBLY FOR IT. 
  TIP: YOU CAN MODULARIZE BETTER AND ADD NEW SMALLER FUNCTIONS IF YOU 
  WANT THAT CAN BE CALLED FROM HERE.
 ************************************************************************
 */  
void ProcessStatements(NodeList* statements) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */    
    while (statements != NULL) {
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */   
        Node* statement = statements->node;
        switch (statement->stmtCode) {
            case ASSIGN:
                HandleAssignmentStatement(statement);
                break;
            case RETURN:
                HandleReturnStatement(statement->left);
                break;
            default:
                break;
        }
       
        statements = statements->next;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO DO CODEGEN FOR ALL THE FUNCTIONS IN THE FILE
 ************************************************************************
*/
void Codegen(NodeList* worklist) {
    fptr = fopen("assembly.s", "w+");
    /*
     ****************************************
              TODO : YOUR CODE HERE
     ****************************************
    */
    if (fptr == NULL) {
        printf("\n Could not create assembly file");
        return; 
    }
    while (worklist != NULL) {
      /*
       ****************************************
              TODO : YOUR CODE HERE
       ****************************************
      */
        Node* functionNode = worklist->node;
        InitAsm(functionNode->name);
        CreateRegList();
        PushArgOnStack(functionNode->arguments);
        ProcessStatements(functionNode->statements);
        RetAsm();
        worklist = worklist->next; 
    }
    fclose(fptr);
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS BELOW THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/
void HandleConstantAssignment(Node* rightNode, char* variableName) { //DONE
    fprintf(fptr, "\nmovq $%li, %s", rightNode->value, lastOffsetUsed);
    UpdateVarInfo(variableName, lastOffsetUsed, rightNode->value, true);
}
void HandleVariableAssignment(Node* rightNode) { //DONE
    fprintf(fptr, "\nmovq %s, %s", LookUpVarInfoByName(rightNode->name), lastOffsetUsed);
}
void HandleFunctionCall(Node* rightNode) { //DONE
    NodeList* args = rightNode->arguments;
    PopArgFromStack(args);
    char* funcName = rightNode->left->name;
    fprintf(fptr, "\ncall %s", funcName);
}
void HandleUnaryNegate(Node* rightNode, char* variableName) { // DONE
    Node* target = rightNode->left;
    if (target->exprCode == VARIABLE || target->exprCode == PARAMETER) { // SEPARATE FOR PARAMETER
        fprintf(fptr, "\nmovq %s, %rax", LookUpVarInfoByName(target->name));
        // Perform negation
        fprintf(fptr, "\nnegq %rax");
        // Store result
        fprintf(fptr, "\nmovq %rax, %s", LookUpVarInfoByName(target->name), lastOffsetUsed);
        UpdateVarInfo(target->name, lastOffsetUsed, INVAL, false);
    }
}
void HandleBinary(Node* node) {
    char* leftLoc, *rightLoc;
    Node* leftTarget = node->left;
    Node* rightTarget = node->right;
    
    // retrieve the locations (or values if they are constants) of the operands
    if (leftTarget->exprCode == CONSTANT) {
        // Left operand is constant, right operand is variable
        fprintf(fptr, "\nmovq $%li, %%rax", leftTarget->value);
        rightLoc = LookUpVarInfoByName(rightTarget->name);
        fprintf(fptr, "\nmovq %s, %%rcx", rightLoc);
    } else if (rightTarget->exprCode == CONSTANT) {
        // Right operand is constant, left operand is variable
        leftLoc = LookUpVarInfoByName(leftTarget->name);
        fprintf(fptr, "\nmovq %s, %%rax", leftLoc);
        fprintf(fptr, "\nmovq $%li, %%rcx", rightTarget->value);
    } else {
        // Both operands are variables
        leftLoc = LookUpVarInfoByName(leftTarget->name);
        rightLoc = LookUpVarInfoByName(rightTarget->name);
        fprintf(fptr, "\nmovq %s, %%rax", leftLoc);
        fprintf(fptr, "\nmovq %s, %%rcx", rightLoc);
    }

    switch (node->opCode) {
        case MULTIPLY:
            fprintf(fptr, "\nimulq %%rcx, %%rax");
            break;
        case ADD:
            fprintf(fptr, "\naddq %%rcx, %%rax");
            break;
        case SUBTRACT:
            fprintf(fptr, "\nsubq %%rcx, %%rax");
            break;
        case DIVIDE:
            fprintf(fptr, "\nxorq %%rdx, %%rdx");   // Clear rdx
            fprintf(fptr, "\ncqto");                // Sign-extend rax into rdx:rax
            fprintf(fptr, "\nidivq %%rcx");         // Divide rdx:rax by rcx. Result is in rax
            break;
        case BOR:
            fprintf(fptr, "\norq %%rcx, %%rax");
            break;
        case BAND:
            fprintf(fptr, "\nandq %%rcx, %%rax");
            break;
        case BXOR:
            fprintf(fptr, "\nxorq %%rcx, %%rax");
            break;
        case BSHR:
            fprintf(fptr, "\nsarq %%rcx, %%rax");
            break;
        case BSHL:
            fprintf(fptr, "\nshlq %%rcx, %%rax");
            break;
        default:
            break;
    }
}

void HandleAssignmentStatement(Node* statement) {
    Node* rightNode = statement->right;
    char* variableName = statement->name;
    LongToCharOffset();
    // CONSTANT
    if (rightNode->exprCode == CONSTANT) {
        HandleConstantAssignment(rightNode, variableName);
    } 
    // VARIABLE 
    else if (rightNode->exprCode == VARIABLE || rightNode->exprCode == PARAMETER) {
        void HandleVariableAssignment(Node* rightNode);
    }
    // OPERATION
    else if (rightNode->exprCode == OPERATION) {
        HandleOperation(rightNode, variableName);
    }
    fprintf(fptr, "\nmovq %rax, %s", lastOffsetUsed);
    UpdateVarInfo(variableName, lastOffsetUsed, INVAL, false);
}
void HandleReturnStatement(Node* statement) { 
    Node* returnValue = statement->left;
    if(returnValue->exprCode == CONSTANT) {
        fprintf(fptr, "\nmovq $%li, %rax", returnValue->value);
    }
    else {
        fprintf(fptr, "\nmovq %s, %rax", LookUpVarInfoByName(returnValue->name));   
    }
    // Clean up
    fprintf(fptr, "\nmovq %rbp, %rsp");
}
void HandleOperation(Node* rightNode, char* variableName) { 
    if (rightNode->opCode == NEGATE) {
        HandleUnaryNegate(rightNode, variableName);
    } else if (rightNode->opCode == FUNCTIONCALL){
        HandleFunctionCall(rightNode);
    } else {
        HandleBinary(rightNode);
    }
}
char* LookUpVarInfoByName(char* name) {
    varList = varLast;
    if (varList == NULL) {
        return "";
    }
    while (varList != NULL) {
        if (!varList->isConst && strcmp(name, varList->varName) == 0) {
            return varList->location;
        }
        varList = varList->prev;
    }
    varList = varHead; 
    return "";
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/


