// utils.cpp

#include <Arduino.h>

int precedence(String op) {
  if (op == "+" || op == "-")
    return 1;
  if (op == "*" || op == "/" || op == "%")
    return 2;
  if (op == "^" || op == "_")
    return 3;
  if (op == "!" || op == "--" ||
      (op.length() == 2 && op[0] == 'f' && isdigit(op[1])))
    return 4;
  return 0;
}

bool is_digit(char ch) {
  return isdigit(ch) || ch == 'p' || ch == 'e' || ch == 'r' || ch == 'a';
}

bool isnumber(String expression) {
  if (expression.length() == 0)
    return false;

  bool hasDecimal = false;
  int start = (expression[0] == '-') ? 1 : 0;

  for (int i = start; i < expression.length(); i++) {
    char c = expression[i];
    if (c == '.') {
      if (hasDecimal)
        return false;
      hasDecimal = true;
    } else if (!isdigit(c)) {
      return false;
    }
  }

  return start < expression.length();
}

String wrap(String text) {
  String res = "";
  if (text.length() > 16) {
    for (int i = text.length() - 16; i < text.length(); i++) {
      res += text[i];
    }
    return res;
  }
  return text;
}