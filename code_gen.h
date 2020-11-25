/* IFJ20 - Code generator
 * Authors:
 * Juraj Marticek, xmarti97
 * Mario Harvan, xharva03
 */
#ifndef CODEGEN_H
#define CODEGEN_H
#include <stdbool.h>
#include "scanner.h"
#include "vector.h"

typedef enum
{
    GLOBAL_FRAME,
    LOCAL_FRAME,
    TEMP_FRAME
} FrameType;

typedef struct
{
    String name;
    FrameType frame;
} Var;

typedef struct
{
    Var var;
    Token token;
    bool isVar;
} Symb;

void gen_init();

//MOVE ⟨var⟩ ⟨symb⟩
void MOVE(Var dest, Symb src, Vector *varScopeVec);

//CREATEFRAME
void CREATEFRAME();

//PUSHFRAME
void PUSHFRAME();

//POPFRAME
void POPFRAME();

//DEFVAR ⟨var⟩
void DEFVAR(Var id, Vector *varScopeVec);

//CALL ⟨label⟩
void CALL(char *id);

//RETURN
void RETURN();

/*_______________DATA_STACK_OPERATIONS________________*/

//PUSHS ⟨symb⟩
void PUSHS(Symb op, Vector *varScopeVec);

//POPS ⟨var⟩
void POPS(Var dest, Vector *varScopeVec);

//CLEARS
void CLEARS();

/*_______________ARITHMETIC_OPERATIONS________________*/

//ADD ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void ADD(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

//SUB ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void SUB(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

//MUL ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void MUL(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

//DIV ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void DIV(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

//IDIV ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void IDIV(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

/*_________________LOGICAL_OPERATIONS_________________*/

//LT ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void LT(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

//GT ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void GT(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

//EQ ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void EQ(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

//AND ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void AND(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

//OR ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void OR(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

//NOT ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void NOT(Var dest, Symb op1, bool useStack, Vector *varScopeVec);

/*______________TYPE_CASTING_OPERATIONS_______________*/

//INT2FLOAT ⟨var⟩ ⟨symb⟩
void INT2FLOAT(Var dest, Symb op, bool useStack, Vector *varScopeVec);

//FLOAT2INT ⟨var⟩ ⟨symb⟩
void FLOAT2INT(Var dest, Symb op, bool useStack, Vector *varScopeVec);

//INT2CHAR ⟨var⟩ ⟨symb⟩
void INT2CHAR(Var dest, Symb op, bool useStack, Vector *varScopeVec);

//STRI2INT ⟨var⟩ ⟨symb⟩
void STRI2INT(Var dest, Symb op1, Symb op2, bool useStack, Vector *varScopeVec);

// supportive function for built-in function print()
void PRINT(Symb symb, Vector *varScopeVec);

/*_______________FUNCTION_OPERATIONS________________*/
void declare_function(char *name, Vector *params, Vector *varScopeVec);

void call_function(char *name, Vector *call_params, Vector *return_params, Vector *varScopeVec);

void return_function(Vector *return_params, Vector *varScopeVec);

void print_i(const char *fmt, ...);

/*_______________DATA_TYPES_OPERATIONS________________*/

//TYPE ⟨var⟩ ⟨symb⟩
void TYPE(Var dest, Symb op, Vector *varScopeVec);

/*_______________STRING_OPERATIONS________________*/

//CONCAT ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void CONCAT(Var dest, Symb op1, Symb op2, Vector *varScopeVec);

//STRLEN ⟨var⟩ ⟨symb⟩
void STRLEN(Var dest, Symb op, Vector *varScopeVec);

//GETCHAR ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void GETCHAR(Var dest, Symb op1, Symb op2, Vector *varScopeVec);

//SETCHAR ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
void SETCHAR(Var dest, Symb op1, Symb op2, Vector *varScopeVec);

/*_______________CONDITIONALS________________*/
// This is the first part of if
void if_start(char *result, Vector *varScopeVec);

// AFTER if_core goes else branch (or if_end immediately)
void if_core();

// This is the last part of if else chain
void if_end();

// Inital for part, remeber to have all needed variables in a frame
void for_start();

void for_expression(Var res, Vector *varScopeVec);

void for_assign_start();

// Body of a for cycle, `res` is result of a test expression
void for_body();

// Final part of the for cycle
void for_end();

// Call this at the end of the main
void main_end();

// This should be the last function called 
void program_end();

void define_var(Var var, Symb value, Vector *varScopeVec);

#endif