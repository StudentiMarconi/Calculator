// symbols.h

#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <Arduino.h>

/// Root symbol
byte root[] = {B00111, B00100, B00100, B00100, B10100, B01100, B00100, B00000};

/// Pi symbol
byte pi[]{
    B00000, B00000, B11111, B01010, B01010, B01010, B01010, B10011,
};

/// Base 2 symbol (for log2)
byte base2[]{
    B00000, B00000, B00000, B01100, B00010, B00100, B01000, B01110,
};

/// Base 10 symbol (for log10)
byte base10[] = {
    B00000, B00000, B00000, B10111, B10101, B10101, B10101, B10111,
};

#endif