#include <LiquidCrystal.h>
#include <Keypad.h>

// Maximum tokens during processing
#define MAX_TOKENS 32
#define OP_TIME 500

//////////////////////////////
/// === INITIALIZATION === ///
//////////////////////////////

// Initialize the LCD
LiquidCrystal lcd(0, 1, 2, 3, 4, 5);

// Keypad rows
const byte ROWS = 4;
// Keypad columns
const byte COLS = 4;
// Keypad buttons
char keys[ROWS][COLS] = {
  { '1', '2', '3', '+' },
  { '4', '5', '6', '-' },
  { '7', '8', '9', '*' },
  { 'C', '0', '=', '/' }
};

// Initialize keypad
byte rowPins[ROWS] = { 13, 12, 14, 27 };
byte colPins[COLS] = { 26, 25, 33, 32 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

///////////////////////
/// === SYMBOLS === ///
///////////////////////

// Root symbol
byte root[] = {
  B00111,
  B00100,
  B00100,
  B00100,
  B10100,
  B01100,
  B00100,
  B00000
};

// Pi symbol
byte pi[] = {
  B00000,
  B00000,
  B11111,
  B01010,
  B01010,
  B01010,
  B01010,
  B10011,
};

// Base 2 symbol (for log2)
byte base2[] = {
  B00000,
  B00000,
  B00000,
  B01100,
  B00010,
  B00100,
  B01000,
  B01110,
};

// Base 10 symbol (for log10)
byte base10[] = {
  B00000,
  B00000,
  B00000,
  B10111,
  B10101,
  B10101,
  B10101,
  B10111,
};

String operation = "";  // Stores the operation being typed by the user
String result = "";     // Stores the result of the operation

///////////////////////////////
/// === DATA STRUCTURES === ///
///////////////////////////////

template<typename T>
struct Stack {
  T data[MAX_TOKENS];
  int top;

  Stack() {
    top = 0;
  }

  void push(T x) {
    data[top++] = x;
  }

  T pop() {
    return data[--top];
  }

  T get(int i) {
    if (i < 0) i += top;
    return data[i];
  }

  bool isEmpty() {
    return top == 0;
  }
};

struct Queue {
  String data[MAX_TOKENS];
  int front;
  int back;

  Queue() {
    front = 0;
    back = 0;
  }

  void push(String x) {
    data[back++] = x;
  }

  String pop() {
    return data[front++];
  }

  bool isEmpty() {
    return (back - front) <= 0;
  }
};

/////////////////////////
/// === FUNCTIONS === ///
/////////////////////////

// Returns the precedence of operators and unary functions
int precedence(String op) {
  // + and -
  if (op == "+" || op == "-") return 1;
  // *, / and %
  if (op == "*" || op == "/" || op == "%") return 2;
  // ^ (power) and _ (root)
  if (op == "^" || op == "_") return 3;
  // ! (factorial), -- (unary minus) and functions (f0...f9)
  if (op == "!" || op == "--" || (op.length() == 2 && op[0] == 'f' && isdigit(op[1]))) return 4;
  // others
  return 0;
}

// Returns wherher ch is a digit or evaluates to one
bool _isdigit(char ch) {
  return isdigit(ch) || ch == 'p' || ch == 'e' || ch == 'r' || ch == 'a';
}

// Returns whether `expression` is a number
bool isnumber(String expression) {
  if (expression.length() == 0) return false;

  bool hasDecimal = false;
  int start = (expression[0] == '-') ? 1 : 0;  // handle negative

  for (int i = start; i < expression.length(); i++) {
    char c = expression[i];
    if (c == '.') {
      if (hasDecimal) return false;  // multiple dots = sus
      hasDecimal = true;
    } else if (!isdigit(c)) {
      return false;
    }
  }

  return start < expression.length();  // avoid "-" only
}

//////////////////////////
/// === CONVERSION === ///
//////////////////////////

// Converts a mathematical into a list of tokens
/* Return codes:
 * 0 - success
 * 1 - number malformed
 * 2 - unknown token
 * 3 - invalid parentheses
 */
int tokenize(String expression, String tokens[]) {
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
        if (isdigit(ch)) num += ch;
        // If the current character is a point, and no other point was found before, add it to the number
        // If a point as already found before, return 1
        else if (ch == '.') {
          if (pt) return 1;
          else {
            num += ch;
            pt = true;
          }
        }
        // If the next character is not a digit nor a point, stop parsing the number
        else
          break;
      }
      // If the last character in the number is a point, return 1
      if (num.endsWith(".")) return 1;
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
      tokens[index++] = isnumber(result) ? result : "0";
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
    // Unary minus. Occurs if the current character is '-' and one of the following:
    /// It's at the beginning of the expression
    /// The previous token was an open parentheses
    /// The previous token was a not number (so it did not begin with a digit or '.')
    else if (ch == '-' && (index == 0 || tokens[index - 1] == "(" || !(tokens[index - 1] != ")" && (isdigit(tokens[index - 1][0]) || tokens[index - 1][0] == '.'))))
      tokens[index++] = "--";
    // '+', '-', '*', '/', '^' (power), '_' (root), '%', '!'
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '_' || ch == '%' || ch == '!')
      tokens[index++] = ch;
    // If any other character was found, return 2
    else return 2;
  }

  // If the number of parentheses does not match, return 3
  if (pCount != 0) return 3;

  return 0;
}

// Arranges a list of tokens into a queue, converting the infix expression into suffix notation
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
    // If the current token is ")", keep popping operators until "(" is encountered or there aren't any operators left, and add all operators (in reverse order) to the result
    else if (token == ")") {
      while (operators.top && operators.get(-1) != "(")
        suffix.push(operators.pop());
      if (operators.top && operators.get(-1) == "(") operators.pop();
    }
    // For all other tokens
    else {
      // Whether the current operator is right associative (unary minus, functions)
      bool isRightAssoc = (token == "--" || (token.length() == 2 && token[0] == 'f' && isdigit(token[1])));

      /*
       * Keep popping operators until a right associative operator close to an operator of equal or higher precedence is found, 
       * a non right associative operator close to an operator of strictly equal precedence is found,
       * or there aren't any operators left. 
       * Then, add all operators (in reverse order) to the result.
       * Comically large comment.
       */
      while (operators.top && ((isRightAssoc && precedence(token) < precedence(operators.get(-1))) || (!isRightAssoc && precedence(token) <= precedence(operators.get(-1)))))
        suffix.push(operators.pop());

      // Add the current token to the operators
      operators.push(token);
    }
  }

  // Pop all remaining operators and add them to the result
  while (operators.top)
    suffix.push(operators.pop());
}

// Evaluates the result of an expression in suffix notation
int evaluate(Queue &suffix, String &result) {
  // Temporary stack used to evaluate the expression
  // It will only contain the result after processing
  Stack<double> proc;

  while (!suffix.isEmpty()) {
    // Continue processing tokens until there are none left
    String token = suffix.pop();

    // If the current token is a number, convert it to a double and push it to the processing stack
    if (isdigit(token[0]) || token[0] == '.')
      proc.push(token.toDouble());
    // "!"
    else if (token == "!") {
      // If there are no numbers to apply the factorial to, return 4
      if (proc.top < 1) return 4;
      // Pop the number to apply the factorial to
      double x = proc.pop();
      // If the number is lower than 0, is not an integer or is greater than 100, return 4
      if (x < 0 || floor(x) != x || x > 100) return 4;
      // Compute the factorial
      double fact = 1;
      for (int i = 1; i <= (int)x; i++) fact *= i;
      // Push the result
      proc.push(fact);
    }
    // If the token is "--" (unary negation) or a function ('f0'..'f9')
    else if (token.length() == 2 && (token == "--" || (token[0] == 'f' && isdigit(token[1])))) {
      // If there is no number to apply the negation or function to, return 5
      if (proc.top < 1) return 5;

      double x = proc.pop();

      // -- (unary negation)
      if (token == "--") x = -x;
      // f1 (sin)
      else if (token == "f1") x = sin(x);
      // f2 (cos)
      else if (token == "f2") x = cos(x);
      // f3 (tan)
      else if (token == "f3") x = tan(x);
      // f4 (ln)
      else if (token == "f4") x = log(x);
      // f5 (log2)
      else if (token == "f5") x = log(x) / log(2);
      // f6 (log10)
      else if (token == "f6") x = log10(x);
      // f7 (abs)
      else if (token == "f7") x = fabs(x);
      // f8 (floor)
      else if (token == "f8") x = floor(x);
      // f9 (ceil)
      else if (token == "f9") x = ceil(x);
      // f0 (sgn)
      else if (token == "f0") x = (x == 0) ? 0 : 1 * (x / fabs(x));

      proc.push(x);
    }
    // For other (binary) tokens
    else {
      // If there are less then 2 numbers to apply the binary operation to, return 6
      if (proc.top < 2) return 6;
      // Pop 2 numbers
      double y = proc.pop();
      double x = proc.pop();

      // +
      if (token == "+") x += y;
      // -
      else if (token == "-") x -= y;
      // *
      else if (token == "*") x *= y;
      // /
      else if (token == "/") {
        // Return 7 in case of division by zero
        if (y == 0) return 7;
        else x /= y;
      }
      // % (modulo)
      else if (token == "%") {
        // Return 7 in case of division by zero
        if (y == 0) return 7;
        else x = fmod(x, y);
      }
      // ^ (power)
      else if (token == "^")
        x = pow(x, y);
      // _ (root)
      else if (token == "_") {
        // Return 7 in case of division by zero
        if (x == 0) return 7;
        x = pow(y, 1.0 / x);
      }
      // Add the resulting number to `proc`
      proc.push(x);
    }
  }
  if (proc.top != 1) return 8;

  result = String(proc.get(0));
  return 0;
}

void calculate() {
  if (operation == "") return;

  String tokens[MAX_TOKENS];
  int retTokenize = tokenize(operation, tokens);
  if (retTokenize) result = "!T" + String(retTokenize);
  else {
    Queue suffix;
    convert(tokens, suffix);
    int retEval = evaluate(suffix, result);
    if (retEval) result = "!E" + String(retEval);
  }
}

//////////////////////
/// === OUTPUT === ///
//////////////////////

/*/ Wraps `text` to fit in 16 characters
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

// Updates the screen and prints `operation` and `result`
void print() {
  lcd.clear();

  String line1 = operation;
  line1.replace("a", "Ans");
  line1 = wrap(line1);
  String line2 = wrap(result);

  lcd.setCursor(16 - line1.length(), 0);
  lcd.print(line1);

  for (int i = 0; i < line1.length(); i++) {
    if (line1[i] == '_') {
      lcd.setCursor(i + 16 - line1.length(), 0);
      lcd.write(byte(0));
    }
  }

  lcd.setCursor(16 - line2.length(), 1);
  lcd.print(line2);
}*/

/////////////////////////
/// === MAIN CODE === ///
/////////////////////////

unsigned long lastTime;
char lastKey;
String variables[10];

void setup() {
  /*
  //lcd.begin(16, 2);
  Serial.begin(9600);
  operation = "5+5";
  String tokens[MAX_TOKENS];
  int res1 = tokenize(operation, tokens);
  Queue suffix;
  convert(tokens, suffix);
  String x;
  int res2 = evaluate(suffix, x);
  //lcd.setCursor(0, 1);
  Serial.print(x);  //lcd
  */
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== INITIALIZE ===");
  lastTime = millis();
  lastKey = ' ';
  /*operation = "f8(5/2)";
  calculate();
  Serial.println(result);*/
}

char opTable[3][4] = {
  { '+', '-', '*', '/' },
  { '(', '.', '^', '_' },
  { '$', '#', '!', '%' },
};

char consts[4] = { 'p', 'e', 'r', 'a' };

void loop() {
  char key = keypad.getKey();
  if (key != NO_KEY) {
    bool opPress = key == lastKey && millis() - lastTime < OP_TIME;
    int lastOp = operation.length() - 1;

    if ((key == 'C' || key == '3') && operation.length()) {  // TODO: key is not 3 bruh
      operation = operation.substring(0, operation.length() - 1);
    } else if (key == '=') {
      if (operation.contains("$$")) {
        
      }
      if (operation.length()==2 && isdigit(operation[0]) && operation[1]=='$') {
        variables[operation.substring(1, operation.length()).toInt()] = result;
        result = "$"+String(operation.substring(1, operation.length()))+"="+operation;
      } else {
        String tmpOp = operation;
        for (int i=0; i<10; i++) {
          operation.replace("$"+i, variables[i]);
        }
        calculate();
      }
    } else if (key == '+' || key == '-' || key == '*' || key == '/') {
      int id = (key == '+')   ? 0
               : (key == '-') ? 1
               : (key == '*') ? 2
                              : 3;
      if (operation[lastOp] == '#')
        operation[lastOp] = consts[id];
      else if (opPress) {
        if (operation[lastOp] == opTable[0][id]) {
          if (opTable[1][id] == '(' && lastOp && !_isdigit(operation[lastOp])) operarion[lastOp] = ')';
          else operation[lastOp] = opTable[1][id];
        } else if (operation[lastOp] == opTable[1][id])
          operation[lastOp] = opTable[2][id];
      } else operation += opTable[0][id];
    } else if (isdigit(key) || key == '#') {
      operation += key;
    }

    lastTime = millis();
    lastKey = key;

    String line1, line2;
    line1 = operation;
    line1 = wrap(line1);
    line2 = result;
    line2 = wrap(result);
    Serial.println("##################");
    Serial.print("#");
    for (int i = 0; i < 16 - line1.length(); i++) Serial.print(" ");
    Serial.println(line1 + "#");
    Serial.print("#");
    for (int i = 0; i < 16 - line2.length(); i++) Serial.print(" ");
    Serial.println(line2 + "#");
    Serial.println("##################");
  }
}
