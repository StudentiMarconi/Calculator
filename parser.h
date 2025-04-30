// parser.h

#ifndef PARSER_H
#define PARSER_H

#include "datastructures.h"
#include <Arduino.h>

/// Converts a mathematical expression into a list of tokens.
///
/// @param expression The expression to tokenize
/// @param tokens The array inside of which the tokens will be stored
/// @param answer The last computed answer, used to replace `Ans` in the
/// expression if present
///
/// @return The operation's return code
///
/// @retval 0 Success
/// @retval 1 Number malformed
/// @retval 2 Unknown token
/// @retval 3 Invalid parentheses
int tokenize(String expression, String tokens[], String answer);

/// Arranges a list of tokens into a queue, converting the infix expression into
/// suffix notation
///
/// @param tokens The array of tokens to convert
/// @param suffix The queue inside of which the resulting suffix notation will
/// be stored
///
/// @note This function assumes the list of tokens is valid and generated from `tokenize()`.
void convert(String tokens[], Queue &suffix);

/// Evaluates the result of a suffix notation expression
///
/// @param suffix The queue containing the suffix expression to evaluate
/// @param result The string where the result will be stored
///
/// @return The operation's return code
///
/// @retval 0 success
/// @retval 4 factorial error
/// @retval 5 unary operation error
/// @retval 6 binary operation error
/// @retval 7 division by zero
/// @retval 8 result stack contains multiple numbers
int evaluate(Queue &suffix, String &result);

/// Calculates the value of an expression
/// @param expression The expression to calculate
/// @param answer The last computed answer, used to replace `Ans` in the
/// expression if present
///
/// @return The expression's result
String calculate(String expression, String answer);

#endif
