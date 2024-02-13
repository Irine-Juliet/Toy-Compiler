#include "optimizer.h"


void Optimizer(NodeList *funcdecls) {
/*
*************************************
     TODO: YOUR CODE HERE
*************************************
*/
bool optimizationMade = true;
    while (optimizationMade) {
        optimizationMade = false;

        // Perform Constant Folding
        if (ConstantFolding(funcdecls)) {
            optimizationMade = true;
        }

        // Perform Constant Propagation
        if (ConstProp(funcdecls)) {
            optimizationMade = true;
        }

        //Perform Dead Code Elimination
        if (DeadAssign(funcdecls)) {
            optimizationMade = true;
        }

    }
}