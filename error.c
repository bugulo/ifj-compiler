/* IFJ20 - Error lib
 * Authors:
 * Juraj Marticek, xmarti97
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

char *errTypeToString(ErrorTypes err_no)
{
  switch (err_no)
  {
  case 1:
    return "LEXICAL ERROR";
    break;
  case 2:
    return "SYNTAX ERROR";
    break;
  case 3:
    return "DEFINITION ERROR";
    break;
  case 4:
    return "TYPE DEFINITION ERROR";
    break;
  case 5:
    return "INCOMPATIBLE EXPRESSION ERROR";
    break;
  case 6:
    return "FUNCTION DEFINITION ERROR";
    break;
  case 7:
    return "SEMANTIC ERROR - OTHER";
    break;
  case 9:
    return "DIVISION BY ZERO";
    break;
  case 99:
    return "INTERNAL ERROR";
    break;
  default:
    return "UNKNOWN ERROR";
    break;
  }
}
