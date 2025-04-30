// datastructures.h

#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include "constants.h"
#include <Arduino.h>

/// Basic implementation of a stack.
/// @tparam T Type of elements stored in the stack.
template <typename T> struct Stack {
  // Static array to store the values
  T data[MAX_TOKENS];

  // Top/latest inserted element of the stack.
  // Also represents the stack's length.
  int top;

  Stack() { top = 0; }

  /// Pushes an element into the top of the stack.
  /// @param x The element to push
  void push(T x) {
    if (top < MAX_TOKENS)
      data[top++] = x;
  }

  /// Pops and returns the top element from the stack.
  /// @return The popped element
  T pop() {
    if (top > 0)
      return data[--top];
  }

  /// Returns an element at a given index. Supports negative indexes.
  /// @param i Element index
  /// @return The selected element
  T get(int i) {
    // If the index is negative, sum it to `top`.
    // e.g. if `top` is 5 and `i` is -1, the element in position 4 will be
    // returned.
    if (i < 0)
      i += top;
    return data[i];
  }

  /// Checks whether the stack is empty.
  /// @return Whether the stack is empty
  bool isEmpty() { return top == 0; }
};

/// Basic implementation of a queue.
struct Queue {
  // Static array to store the values
  String data[MAX_TOKENS];

  // First and last elements in the queue
  // The queue ranges from front (included) to back (excluded)
  int front, back;

  Queue() {
    front = 0;
    back = 0;
  }

  /// Pushes an element to the back of the queue.
  /// @param x The element to push
  void push(String x) {
    if (back < MAX_TOKENS)
      data[back++] = x;
  }

  /// Pops and returns an element from the front of the queue.
  /// @return The popped element
  String pop() {
    if (front < back)
      return data[front++];
  }

  /// Checks whether the queue is empty.
  /// @return Whether the queue is empty
  bool isEmpty() { return back == front; }
};

#endif