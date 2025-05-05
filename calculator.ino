#include "constants.h"
#include "parser.h"
#include "symbols.h"
#include "utils.h"
//#include <Keypad.h>
#include <LiquidCrystal.h>

// Initialize the LCD
LiquidCrystal lcd(35, 32, 33, 25, 26, 27);

// Keypad rows
const byte ROWS = 4;
// Keypad columns
const byte COLS = 4;
// Keypad buttons
char keys[ROWS][COLS] = {{'1', '2', '3', '+'},
                         {'4', '5', '6', '-'},
                         {'7', '8', '9', '*'},
                         {'C', '0', '=', '/'}};

// Initialize keypad
byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};
//Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String operation = ""; // Stores the operation being typed by the user
String result = "";    // Stores the result of the operation

unsigned long lastTime = millis();
char lastKey = ' ';
String variables[10];

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(0, root);
  lcd.createChar(1, pi);
  lcd.createChar(2, base2);
  lcd.createChar(3, base10);
}

void loop() {
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Sei gay");
}

char opTable[3][4] = {
    {'+', '-', '*', '/'},
    {'(', '.', '^', '_'},
    {'$', '#', '!', '%'},
};

char consts[4] = {'p', 'e', 'r', 'a'};

String funcs[10] = {
    "sgn",  "sin",   "cos",  "tan", "ln",
    "logB", // log2
    "logD", // log10
    "abs",  "floor", "ceil",
};

/*
void loop() {
  // Get current pressed key
  char key = keypad.getKey();

  // If a key was pressed
  if (key != NO_KEY) {
    // Whether the key was pressed within OP_TIME milliseconds before the
    // previous one, and the previous key is the same as the current one
    bool opPress = key == lastKey && millis() - lastTime < OP_TIME;
    // The position of the last character in the operation
    int lastOp = operation.length() - 1;

    // If 'C' was pressed
    if (key == 'C') {
      // If the operation lasts with a function ('fX'), strip 2 characters off
      // of the operation
      if (lastOp && isdigit(operation[lastOp]) && operation[lastOp - 1] == 'f')
        operation = operation.substring(0, operation.length() - 2);
      // If the operation is not empty, strip 1 character off of it
      else if (operation.length())
        operation = operation.substring(0, operation.length() - 1);
    }
    // If '=' was pressed
    else if (key == '=') {
      // If the current operation is 'X$'
      if (operation.length() == 2 && isdigit(operation[0]) &&
          operation[1] == '$') {
        // Assign the latest computed value to the variable '$X', then display
        // this change
        variables[operation.substring(0, 1).toInt()] = result;
        result = result + "=$" + operation[0];
      }
      // If no variable must be modified
      else {
        // Calculate the expression and display the result
        String tmpOp = operation;
        // Replace all variables with their values
        for (int i = 0; i < 10; i++) {
          tmpOp.replace("$" + i, variables[i]);
        }
        result = calculate(tmpOp, result);
      }
    }
    // If an operator was pressed
    else if (key == '+' || key == '-' || key == '*' || key == '/') {
      int id = (key == '+') ? 0 : (key == '-') ? 1 : (key == '*') ? 2 : 3;
      // If the last character was '#'
      if (lastOp >= 0 && operation[lastOp] == '#')
        // Pressing an operator after '#' will replace that with a constant.
        // #+ -> pi
        // #- -> e
        // #* -> rnd
        // #/ -> ans
        operation[lastOp] = consts[id];
      // If a key was pressed multiple times in less than OP_TIME milliseconds
      else if (opPress) {
        if (lastOp >= 0 && operation[lastOp] == opTable[0][id]) {
          if (opTable[1][id] == '(' && lastOp && !is_digit(operation[lastOp]))
            operation[lastOp] = ')';
          else
            operation[lastOp] = opTable[1][id];
        } else if (lastOp >= 0 && operation[lastOp] == opTable[1][id])
          operation[lastOp] = opTable[2][id];
      } else
        operation += opTable[0][id];
    } else if (isdigit(key)) {
      // Pressing a digit after '#' will replace it with `f`, thus creating a
      // function
      if (lastKey == '=' && result.length() && result[0]!='!') operation = key;
      else {
        if (lastOp >= 0 && operation[lastOp] == '#')
          operation[lastOp] = 'f';
        operation += key;
      }
    }

    lastTime = millis();
    lastKey = key;

    lcd.clear();
    String line1 = operation, line2 = result;
    for (int i = 0; i < 10; i++) {
      line1.replace("f" + String(i), funcs[i]);
    }
    line1.replace("a", "Ans");
    line1.replace("r", "Rnd");
    line1 = wrap(line1);
    lcd.setCursor(15 - line1.length(), 0);
    for (int i = 0; i < line1.length(); i++) {
      if (line1[i] == '_') {
        lcd.write(byte(0));
      } else if (line1[i] == 'p') {
        lcd.write(byte(1));
      } else if (line1[i] == 'B') {
        lcd.write(byte(2));
      } else if (line1[i] == 'D') {
        lcd.write(byte(3));
      } else {
        lcd.print(line1[i]);
      }
    }
    line2 = wrap(line2);
    lcd.setCursor(15 - line2.length(), 1);
    lcd.print(line2);
  }
}
*/
