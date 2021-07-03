// This is the definition of the prototypes defined in the header file
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "functions.h"


// default function for deallocating a function part
// with this function you assert that every child is listed in the linked list of the parent
void default_free(FUNC_P fp) {
    if(fp -> numChilds > 0) {
        free(fp -> childs);
    }
    free(fp);
}

// allocates a new default function part with a fixed amount of childs
// this function initializes the function part with initials values
FUNC_P alloc_fp(unsigned int childs) {
    FUNC_P fp = (FUNC_P) malloc(sizeof(FunctionPart));
    if(fp == NULL) {
        free(fp);
        return NULL;
    }
    fp -> numChilds = childs;
    fp -> free = default_free;
    fp -> allParts.first = NULL;
    fp -> allParts.size = 0;
    if(!childs) return fp;
    fp -> childs = malloc(childs * sizeof(void *));
    if(fp -> childs == NULL) {
        free(fp -> childs);
        free(fp);
        return NULL;
    }
    return fp;
}

// this function adds an item to an linked list if that item does not already exist in the list
// this function is used by transfering items from the child elements to the parents
// this function is important because no duplicates will created in the instantiation of the parent
// this is important because deallocating the members will not cause an error
void addIfNotExists(LinkedList * target, void * item) {
    LinkedListItem * current = target -> first;
    LinkedListItem * last = current;
    while(current != NULL) {
        if(current -> item == item) {
            return;
        }
        last = current;
        current = last -> next;
    }
    LinkedListItem * new = (LinkedListItem *) malloc(sizeof(LinkedListItem));
    if(new == NULL) return;
    new -> next = NULL;
    new -> item = item;
    last -> next = new;
    target -> size++;
}

// This function transfers all members from the child to the parent
// it is at instantiation and is important for deallocating the members of a function
// it is important that no duplicates will appear in the linked list with all members of the parent
void transferToParent(FUNC_P parent, FUNC_P child) {
    LinkedListItem * nextC = child -> allParts.first;
    while(nextC != NULL) {
        addIfNotExists(&(parent -> allParts), nextC -> item);
        LinkedListItem * temp = nextC;
        nextC = nextC -> next;
        free(temp);
    }
    child -> allParts.size = 0;
    child -> allParts.first = NULL;
}

// this method deallocates all members of a function
// it will iterate over the linked list of the parent and calls the free-Method of an item
void free_all(FUNC_P function) {
    LinkedListItem * next = function -> allParts.first;
    while(next != NULL) {
        LinkedListItem * temp = next;
        next = next -> next;
        ((FUNC_P)(temp -> item)) -> free(temp -> item);
    }
    function -> free(function);
}

// function for returning the value of a variable
double varFunction(FUNC_P fp) {
    return VAR_VAL(fp);
}

// The derivative of a single variable
double varDerivative(FUNC_P target, FUNC_P fp) {
    return target == fp ? 1 : 0;
}

// custom free function for the var element
void freeVar(FUNC_P fp) {
    free(CHILD_EL(fp, 0, double));
    free(fp -> childs);
    free(fp);
}

// Function for creating a variable
FUNC_P var(double value) {
    FUNC_P func_p = alloc_fp(1);
    if(func_p == NULL) return NULL;
    double * valp = (double *) malloc(sizeof(double));
    if(valp == NULL) {
        free(func_p -> childs);
        free(func_p);
        return NULL;
    }
    *valp = value;
    CHILD_EL(func_p, 0, double) = valp;
    func_p -> function = varFunction;
    func_p -> derivative = varDerivative;
    func_p -> free = freeVar;
    return func_p; 
}

// function for returning the addition of two members
double addFunction(FUNC_P fp) {
    return CHILD_VAL(fp, 0) + CHILD_VAL(fp, 1);
}

// function for returning the derivatove of the addition of two members
double addDerivative(FUNC_P target, FUNC_P fp) {
    return CHILD_DER(target, fp, 0) + CHILD_DER(target, fp, 1);
}

// this will add two function parts to a sum
FUNC_P add(FUNC_P a, FUNC_P b) {
    FUNC_P funcp = alloc_fp(2);
    if(funcp == NULL) return NULL;
    CHILD_FP(funcp, 0) = a;
    CHILD_FP(funcp, 1) = b;
    transferToParent(funcp, a);
    transferToParent(funcp, b);
    funcp -> function = addFunction;
    funcp -> derivative = addDerivative;
    return funcp;
}

// function for subtracting two members
double subFunction(FUNC_P fp) {
    return CHILD_VAL(fp, 0) - CHILD_VAL(fp, 1);
}

// this function will return the derivative of the subtraction of two members
double subDerivative(FUNC_P target, FUNC_P fp) {
    return CHILD_DER(target, fp, 0) - CHILD_DER(target, fp, 1);
}

// this function will subtract two function parts
FUNC_P sub(FUNC_P a, FUNC_P b) {
    FUNC_P funcp = alloc_fp(2);
    if(funcp == NULL) return NULL;
    CHILD_FP(funcp, 0) = a;
    CHILD_FP(funcp, 1) = b;
    transferToParent(funcp, a);
    transferToParent(funcp, b);
    funcp -> function = subFunction;
    funcp -> derivative = subDerivative;
    return funcp;
}

// function for multiplying two members
double mulFunction(FUNC_P fp) {
    return CHILD_VAL(fp, 0) * CHILD_VAL(fp, 1);
}

// this function will return the derivation of the multiplication of two members
double mulDerivative(FUNC_P target, FUNC_P fp) {
    return CHILD_VAL(fp, 0) * CHILD_DER(target, fp, 1) + CHILD_VAL(fp, 1) * CHILD_DER(target, fp, 0);
}

// this function will multiply two function parts with each other
FUNC_P mul(FUNC_P a, FUNC_P b) {
    FUNC_P funcp = alloc_fp(2);
    if(funcp == NULL) return NULL;
    CHILD_FP(funcp, 0) = a;
    CHILD_FP(funcp, 1) = b;
    transferToParent(funcp, a);
    transferToParent(funcp, b);
    funcp -> function = mulFunction;
    funcp -> derivative = mulDerivative;
    return funcp;
}

// function for the devisiion between two members
double divFunction(FUNC_P fp) {
    return CHILD_VAL(fp, 0) / CHILD_VAL(fp, 1);
}

// function for the derivative of the devision of two members
double divDerivative(FUNC_P target, FUNC_P fp) {
    double g = CHILD_VAL(fp, 1);
    return (g * CHILD_DER(target, fp, 0) - CHILD_VAL(fp, 0) * CHILD_DER(target, fp, 1)) / (g * g);
}

// function for the devision between two function parts
FUNC_P divfp(FUNC_P a, FUNC_P b) {
    FUNC_P funcp = alloc_fp(2);
    if(funcp == NULL) return NULL;
    CHILD_FP(funcp, 0) = a;
    CHILD_FP(funcp, 1) = b;
    transferToParent(funcp, a);
    transferToParent(funcp, b);
    funcp -> function = divFunction;
    funcp -> derivative = divDerivative;
    return funcp;
}

// function for raising e two a value
double expFunction(FUNC_P fp) {
    return exp(CHILD_VAL(fp, 0));
}

// derivation of an exponential function
double expDerivative(FUNC_P target, FUNC_P fp) {
    return CHILD_DER(target, fp, 0) * VAL_FP(fp); 
}

// this function will raise e two a value of a function part
FUNC_P expfp(FUNC_P a) {
    FUNC_P funcp = alloc_fp(1);
    if(funcp == NULL) return NULL;
    CHILD_FP(funcp, 0) = a;
    funcp -> function = expFunction;
    funcp -> derivative = expDerivative;
    transferToParent(funcp, a);
    return funcp;
}

// the natural logarithm of a member
double logFunction(FUNC_P a) {
    return log(CHILD_VAL(a, 0));
}

// the derivation of the natural logarithm of a member
double logDerivative(FUNC_P target, FUNC_P fp) {
    return CHILD_DER(target, fp, 0) / CHILD_VAL(fp, 0);
}

// this function will call the natural logarithm on a function part
FUNC_P logfp(FUNC_P a) {
    FUNC_P funcp = alloc_fp(1);
    if(funcp == NULL) return NULL;
    CHILD_FP(funcp, 0) = a;
    funcp -> function = logFunction;
    funcp -> derivative = logDerivative;
    transferToParent(funcp, a);
    return funcp;
}

// the negation of a member
double negFunction(FUNC_P a) {
    return -CHILD_VAL(a, 0);
}

// the derivation of the negation of a member
double negDerivative(FUNC_P target, FUNC_P fp) {
    return -CHILD_DER(target, fp, 0);
}

// this function will negate a function part
FUNC_P neg(FUNC_P a) {
    FUNC_P funcp = alloc_fp(1);
    if(funcp == NULL) return NULL;
    CHILD_FP(funcp, 0) = a;
    funcp -> function = negFunction;
    funcp -> derivative = negDerivative;
    transferToParent(funcp, a);
    return funcp;
}

// this function will deallocate a pow-function because the pow function does include a double
void freePow(FUNC_P fp) {
    free(CHILD_EL(fp, 1, double));
    free(fp -> childs);
    free(fp);
}

// this will raise the member two a power
double powFunction(FUNC_P a) {
    return pow(CHILD_VAL(a, 0), *CHILD_EL(a, 1, double));
}

// this will calculate the derivative of the pow function
double powDerivative(FUNC_P target, FUNC_P fp) {
    return *CHILD_EL(fp, 1, double) * pow(CHILD_VAL(fp, 0), (*CHILD_EL(fp, 1, double)) - 1);
}

// this function will raise a function part two a fixed number
FUNC_P powfp(FUNC_P base, double exponent) {
    FUNC_P funcp = alloc_fp(2);
    if(funcp == NULL) return NULL;
    double * expo = malloc(sizeof(double));
    if(expo == NULL) {
        free(funcp -> childs);
        free(funcp);
        return NULL;
    }
    CHILD_FP(funcp, 0) = base;
    CHILD_EL(funcp, 1, double) = expo;
    funcp -> free = freePow;
    transferToParent(funcp, base);
    return funcp;
}


// this function will make a simple regression on a parameter
void regression(FUNC_P function, FUNC_P parameter, double expected, double learnrate) {
    VAR_VAL(parameter) += learnrate * (expected - VAL_FP(function)) * DER_FP(parameter, function);
}