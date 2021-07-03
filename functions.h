// This is the header file of the functions lib.
// This lib should make it possible to create function which can dynamicly calculate values and derivatives
// The target of this lib is to make a performant and also dynamic regression model
// MADE BY Pohhnii | Christopher Steinwarz

#ifndef FUNCTIONS
#define FUNCTIONS

// definition for a pointer to a function part
// this definition is made because "FunctionPart *"" is called very often
// it could be redefined to an typedef later for more type security
#ifndef FUNC_P
#define FUNC_P FunctionPart *
#endif

// definition for getting the value of a function
#ifndef VAL_FP
#define VAL_FP(f) (f -> function(f))
#endif

// definition for getting the derivative of a function
#ifndef DER_FP
#define DER_FP(target, fp) (fp -> derivative(target, fp))
#endif

#ifndef CHILD_EL
#define CHILD_EL(fp, index, type) (((type **)(fp -> childs))[index])
#endif

// definition for getting a pointer to a child at a specific index position
#ifndef CHILD_FP
#define CHILD_FP(fp, index) CHILD_EL(fp, index, FunctionPart)
#endif

// deinition for getting the calculated value of a child
#ifndef CHILD_VAL
#define CHILD_VAL(fp, index) VAL_FP(CHILD_FP(fp, index))
#endif

// definition for getting the calculated derivation of a child
#ifndef CHILD_DER
#define CHILD_DER(target, fp, index) DER_FP(target, CHILD_FP(fp, index))
#endif

// Definition for getting the pointer to a child element
#ifndef VAR_VAL
#define VAR_VAL(fp) (* CHILD_EL(fp, 0, double))
#endif

// structure for an item in a linked list
typedef struct LinkedListItemStruct {
    void * item;
    struct LinkedListItemStruct * next;
} LinkedListItem;

// structure for a linked list
typedef struct LinkedListStruct {
    LinkedListItem * first;
    int size;
} LinkedList;

// struct for every function part
typedef struct FunctionPartStruct {
    // The childs of the part, which will be used to calculate a value
    void *childs;
    // The number of childs, so that you are able to iterate over the childs
    unsigned int numChilds;
    // The function for calculating the value of this part
    double (* function)(struct FunctionPartStruct *);
    // The function for calculating the derivative of this part
    double (* derivative)(struct FunctionPartStruct *, struct FunctionPartStruct *);
    // The free function for deallocating the memory
    void (* free)(struct FunctionPartStruct *);
    // The parent function contains the references to all function parts
    LinkedList allParts;
} FunctionPart;

// deallocates all allocated members in the heap
void free_all(FUNC_P);

// this will make a simple regression on a parameter
void regression(FUNC_P, FUNC_P, double, double);

/*
FUNCTIONS:
- var
- add
- sub
- mul
- div
- exp
- log
- neg
- pow
*/

// Variable definition
FUNC_P var(double);

// Addition definition
FUNC_P add(FUNC_P, FUNC_P);

// Subtraction definition
FUNC_P sub(FUNC_P, FUNC_P);

// Multiplication definition
FUNC_P mul(FUNC_P, FUNC_P);

// Division definition
FUNC_P divfp(FUNC_P, FUNC_P);

// definition of e to the power of x
FUNC_P expfp(FUNC_P);

// natural logarithm definition
FUNC_P logfp(FUNC_P);

// negation definition
FUNC_P neg(FUNC_P);

// definition of x to the power of a value
FUNC_P powfp(FUNC_P, double);

#endif