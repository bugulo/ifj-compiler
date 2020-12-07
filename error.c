/* IFJ20 - Error lib
 * Authors:
 * Juraj Marticek, xmarti97
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

// Function for converting enum (ErrorTypes) to string
char *errTypeToString(ErrorTypes err_no)
{
  switch (err_no)
  {
  case LEXICAL_ERROR:
    return "LEXICAL ERROR";
    break;
  case SYNTAX_ERROR:
    return "SYNTAX ERROR";
    break;
  case DEFINITION_ERROR:
    return "DEFINITION ERROR";
    break;
  case TYPE_DEFINITION_ERROR:
    return "TYPE DEFINITION ERROR";
    break;
  case INCOMPATIBLE_EXPRESSION_ERROR:
    return "INCOMPATIBLE EXPRESSION ERROR";
    break;
  case FUNCTION_DEFINITION_ERROR:
    return "FUNCTION DEFINITION ERROR";
    break;
  case OTHER_SEMANTIC_ERROR:
    return "SEMANTIC ERROR - OTHER";
    break;
  case ZERO_DIVISION_ERROR:
    return "DIVISION BY ZERO";
    break;
  case INTERNAL_ERROR:
    return "INTERNAL ERROR";
    break;
  default:
    return "UNKNOWN ERROR";
    break;
  }
}
