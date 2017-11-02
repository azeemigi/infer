/*
 * Copyright (c) 2017 - present Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include <new>

namespace dead_stores {

void easy_bad() { int x = 5; }

void reassign_param_bad(int x) { x = 5; }

int dead_then_live_bad() {
  int x = 5;
  x = 3;
  return x;
}

int use_then_dead_bad() {
  int x = 5;
  int y = x;
  x = 7;
  return y;
}

void dead_pointer_bad() {
  int num = 2;
  int* x = &num;
}

void plus_plus1_bad() {
  int i = 1;
  ++i;
}

void plus_plus2_bad() {
  int i = 1;
  i++;
}

int plus_plus3_bad() {
  int i = 1;
  return i++;
}

void FN_capture_no_read_bad() {
  int x = 1;
  [x]() { return; }();
}

void init_capture_reassign_bad() {
  int i = 1; // this is a dead store
  return [i = 1]() { return i; }
  ();
}

void init_capture_no_call_bad() {
  [i = 1]() { return i; };
}

int FN_init_capture_no_read_bad() {
  return [i = 1]() { return 0; }
  ();
}

int return_ok() {
  int x = 5;
  return x;
}

int branch_ok(bool b) {
  int x = 5;
  int y = 3;
  if (b) {
    y = x;
  }
  return y;
}

int loop_ok(bool b) {
  int x = 5;
  int y = 3;
  while (b) {
    y = x;
    b = false;
  }
  return y;
}

int loop_break_ok(bool b) {
  int x = 5;
  while (b) {
    x = 3;
    break;
  }
  return x;
}

int loop_continue_ok(bool b) {
  int x = 5;
  int y = 2;
  while (b) {
    y = x;
    x = 3;
    continue;
  }
  return y;
}

void assign_pointer1_ok(int* ptr) { *ptr = 7; }

int* assign_pointer2_ok() {
  int num = 2;
  int* ptr = &num;
  return ptr;
}

void by_ref1_ok(int& ref) { ref = 7; }

void by_ref2_ok(int& ref) { ref++; }

int capture_by_ref3_ok() {
  int x = 1;
  [&](auto y) { x += y; }(3);
  return x;
}

int plus_plus_ok() {
  int x = 1;
  return ++x;
}

int plus_plus_loop_ok(int n) {
  int i;
  for (i = 1; i < n; i++) {
    i++;
  }
  return i;
}

void lambda_bad() {
  int x = []() {
    int y = 1;
    y = 2;
    return y;
  }();
  return x;
}

void capture1_ok() {
  int x = 1;
  [x]() { return x; }();
}

void capture2_ok(int x) {
  [x]() { return x; }();
}

int capture_by_ref1_ok() {
  int x = 1;
  [&x]() { x++; }();
  return x;
}

int capture_by_ref2_ok() {
  int x = 1;
  int y = 1;
  [&]() {
    x = x + y;
    y = x;
  }();
  return x + y;
}

int FN_capture_by_ref_reuseBad() {
  int x = 1;
  [&x]() {
    x = 1; // dead, but we won't report
    x = 2;
  }();
  return x;
}

int init_capture1_ok() {
  return [i = 1]() { return i; }
  ();
}

int init_capture2_ok() {
  int i = 1;
  return [j = i]() { return j; }
  ();
}

int init_capture3_ok() {
  int i = 1;
  return [i = i]() { return i; }
  ();
}

int init_capture4_ok() {
  int i = 1;
  int j = 1;
  return [ a = 1, b = i, c = j ]() { return a + b + c; }
  ();
}

int init_capture5_ok() {
  int i = 1;
  int k = [j = i]() { return j; }
  ();
  i = 5; // should not be flagged
  return i + k;
}

int init_capture6_ok() {
  int i = 1;
  int k = [i = i + 1]() { return i; }
  ();
  i = 5; // should not be flagged;
  return i + k;
}

char* global;

void FP_assign_array_tricky_ok() {
  char arr[1];
  global = arr;
  *(int*)arr = 123; // think this is a bug in the frontend... this instruction
  // looks like &arr:int = 123
}

void placement_new_ok(int len, int* ptr) {
  int* placement = ptr;
  while (len--) {
    new (placement++) int(5);
  }
}

// we don't report on dead stores where the RHS is 0, 0.0, false, nullptr, etc.

bool sentinel_bool_ok() {
  bool b = false;
  b = true;
  return b;
}

int sentinel_int_ok() {
  int i = 0;
  i = 1;
  return i;
}

int sentinel_long_ok() {
  long l = 0L;
  l = 1L;
  return l;
}

float sentinel_float_ok() {
  float f = 0.0;
  f = 1.0;
  return f;
}

double sentinel_double_ok() {
  double d = 0.0;
  d = 1.0;
  return d;
}

int* sentinel_ptr_ok(int* j) {
  int* i = nullptr;
  i = j;
  return i;
}

}
