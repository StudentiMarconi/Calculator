// utils.h

#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

/// Returns the precedence of operators and unary functions.
/// @param op The operator in question
/// @return The precedence of said operator
int precedence(String op);

/// Returns whether a character is a digit or evaluates to one.
/// @param ch The character in question
/// @return Whether said character is a digit
bool is_digit(char ch);

/// Returns whether an expression represents a number.
/// @param expression The expression in question
/// @return Whether said expression is a number
bool isnumber(String expression);

/// Shrinks some text to fit into 16 characters of an LCD.
/// @param text The text in question
/// @return The resulting text
String wrap(String text);

#endif