// parser.cpp

#include "constants.h"
#include "datastructures.h"
#include "utils.h"
#include <Arduino.h>

int tokenize(String expression, String tokens[], String answer) {
  // Remove all spaces from expression (just in case :P)
  String exp = expression;
  exp.replace(" ", "");

  // Pointer to next usable position in `tokens`
  int index = 0;
  // Length of the expression
  int N = exp.length();
  // Number of open parentheses minus number of closed parentheses
  int pCount = 0;

  for (int i = 0; i < N; i++) {
    // For each character in the expression
    char ch = exp[i];

    // If the character is '.', parse a number
    if (isdigit(ch) || ch == '.') {
      // True if a point is found in the number
      bool pt = false;
      // Resulting number
      String num = "";
      for (; i < N; i++) {
        // Continue parsing next characters
        ch = exp[i];
        // If the current character is a digit, add it to the number
        if (isdigit(ch))
          num += ch;
        // If the current character is a point, and no other point was found
        // before, add it to the number If a point as already found before,
        // return 1
        else if (ch == '.') {
          if (pt)
            return 1;
          else {
            num += ch;
            pt = true;
          }
        }
        // If the next character is not a digit nor a point, stop parsing the
        // number
        else
          break;
      }
      // If the last character in the number is a point, return 1
      if (num.endsWith("."))
        return 1;
      // Add the number to the tokens
      tokens[index++] = num;
      // Decrease `i` to allow processing the character in the next iteration
      i--;
    }
    // 'f0'..'f9' - functions
    else if (i < N - 1 && ch == 'f' && isdigit(exp[i + 1]))
      tokens[index++] = "f" + String(exp[++i]);
    // 'p' - pi
    else if (ch == 'p')
      tokens[index++] = String(M_PI);
    // 'e'
    else if (ch == 'e')
      tokens[index++] = String(M_E);
    // 'r' - random number
    else if (ch == 'r')
      tokens[index++] = String((double)random(1000) / 1000);
    // 'a' - previous answer
    else if (ch == 'a')
      tokens[index++] = isnumber(answer) ? answer : "0";
    // '('
    else if (ch == '(') {
      tokens[index++] = '(';
      // Increase the open parentheses count
      pCount++;
    }
    // ')'
    else if (ch == ')') {
      tokens[index++] = ')';
      // Decrease the open parentheses count
      pCount--;
    }
    // Unary minus. Occurs if the current character is '-' and one of the
    // following:
    /// It's at the beginning of the expression
    /// The previous token was an open parentheses
    /// The previous token was a not number (so it did not begin with a digit or
    /// '.')
    else if (ch == '-' &&
             (index == 0 || tokens[index - 1] == "(" ||
              !(tokens[index - 1] != ")" && (isdigit(tokens[index - 1][0]) ||
                                             tokens[index - 1][0] == '.'))))
      tokens[index++] = "--";
    // '+', '-', '*', '/', '^' (power), '_' (root), '%', '!'
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' ||
             ch == '_' || ch == '%' || ch == '!')
      tokens[index++] = ch;
    // If any other character was found, return 2
    else
      return 2;
  }

  // If the number of parentheses does not match, return 3
  if (pCount != 0)
    return 3;

  return 0;
}

void convert(String tokens[], Queue &suffix) {
  // Temporary stack containing the operators being processed
  Stack<String> operators;

  for (int i = 0; i < MAX_TOKENS && tokens[i] != ""; i++) {
    // For each token in the expression
    String token = tokens[i];

    // Numbers and "!" get pushed directly to the result
    if (isdigit(token[0]) || token[0] == '.' || token == "!")
      suffix.push(token);
    // "(" gets pushed directly to the result
    else if (token == "(")
      operators.push(token);
    // If the current token is ")", keep popping operators until "(" is
    // encountered or there aren't any operators left, and add all operators (in
    // reverse order) to the result
    else if (token == ")") {
      while (!operators.isEmpty() && operators.get(-1) != "(")
        suffix.push(operators.pop());
      if (!operators.isEmpty() && operators.get(-1) == "(")
        operators.pop();
    }
    // For all other tokens
    else {
      // Whether the current operator is right associative (unary minus,
      // functions)
      bool isRightAssoc =
          (token == "--" ||
           (token.length() == 2 && token[0] == 'f' && isdigit(token[1])));

      /*
       * Keep popping operators until a right associative operator close to an
       * operator of equal or higher precedence is found, a non right
       * associative operator close to an operator of strictly equal precedence
       * is found, or there aren't any operators left. Then, add all operators
       * (in reverse order) to the result. Comically large comment.
       */
      while (!operators.isEmpty() &&
             ((isRightAssoc &&
               precedence(token) < precedence(operators.get(-1))) ||
              (!isRightAssoc &&
               precedence(token) <= precedence(operators.get(-1)))))
        suffix.push(operators.pop());

      // Add the current token to the operators
      operators.push(token);
    }
  }

  // Pop all remaining operators and add them to the result
  while (!operators.isEmpty())
    suffix.push(operators.pop());
}

int evaluate(Queue &suffix, String &result) {
  // Temporary stack used to evaluate the expression
  // It will only contain the result after processing
  Stack<double> proc;

  while (!suffix.isEmpty()) {
    // Continue processing tokens until there are none left
    String token = suffix.pop();

    // If the current token is a number, convert it to a double and push it to
    // the processing stack
    if (isdigit(token[0]) || token[0] == '.')
      proc.push(token.toDouble());
    // "!"
    else if (token == "!") {
      // If there are no numbers to apply the factorial to, return 4
      if (proc.top < 1)
        return 4;
      // Pop the number to apply the factorial to
      double x = proc.pop();
      // If the number is lower than 0, is not an integer or is greater than
      // 100, return 4
      if (x < 0 || floor(x) != x || x > 100)
        return 4;
      // Compute the factorial
      double fact = 1;
      for (int i = 1; i <= (int)x; i++)
        fact *= i;
      // Push the result
      proc.push(fact);
    }
    // If the token is "--" (unary negation) or a function ('f0'..'f9')
    else if (token.length() == 2 &&
             (token == "--" || (token[0] == 'f' && isdigit(token[1])))) {
      // If there is no number to apply the negation or function to, return 5
      if (proc.top < 1)
        return 5;

      double x = proc.pop();

      // -- (unary negation)
      if (token == "--")
        x = -x;
      // f1 (sin)
      else if (token == "f1")
        x = sin(x);
      // f2 (cos)
      else if (token == "f2")
        x = cos(x);
      // f3 (tan)
      else if (token == "f3")
        x = tan(x);
      // f4 (ln)
      else if (token == "f4")
        x = log(x);
      // f5 (log2)
      else if (token == "f5")
        x = log(x) / log(2);
      // f6 (log10)
      else if (token == "f6")
        x = log10(x);
      // f7 (abs)
      else if (token == "f7")
        x = fabs(x);
      // f8 (floor)
      else if (token == "f8")
        x = floor(x);
      // f9 (ceil)
      else if (token == "f9")
        x = ceil(x);
      // f0 (sgn)
      else if (token == "f0")
        x = (x == 0) ? 0 : 1 * (x / fabs(x));

      proc.push(x);
    }
    // For other (binary) tokens
    else {
      // If there are less then 2 numbers to apply the binary operation to,
      // return 6
      if (proc.top < 2)
        return 6;
      // Pop 2 numbers
      double y = proc.pop();
      double x = proc.pop();

      // +
      if (token == "+")
        x += y;
      // -
      else if (token == "-")
        x -= y;
      // *
      else if (token == "*")
        x *= y;
      // /
      else if (token == "/") {
        // Return 7 in case of division by zero
        if (y == 0)
          return 7;
        else
          x /= y;
      }
      // % (modulo)
      else if (token == "%") {
        // Return 7 in case of division by zero
        if (y == 0)
          return 7;
        else
          x = fmod(x, y);
      }
      // ^ (power)
      else if (token == "^")
        x = pow(x, y);
      // _ (root)
      else if (token == "_") {
        // Return 7 in case of division by zero
        if (x == 0)
          return 7;
        x = pow(y, 1.0 / x);
      }
      // Add the resulting number to `proc`
      proc.push(x);
    }
  }
  if (proc.top != 1)
    return 8;

  result = String(proc.get(0));
  return 0;
}

String calculate(String expression, String answer) {
  if (expression == "")
    return "";

  String res;

  String tokens[MAX_TOKENS];
  int retTokenize = tokenize(expression, tokens, answer);
  if (retTokenize)
    res = "!T" + String(retTokenize);
  else {
    Queue suffix;
    convert(tokens, suffix);
    int retEval = evaluate(suffix, res);
    if (retEval)
      res = "!E" + String(retEval);
  }

  return res;
}
