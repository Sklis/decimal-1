#include "s21_decimal.h"

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  int ret = 0;
  exp_compar(&value_1, &value_2);
  ret = equal_simple(value_1, value_2);
  if (ret == 1 && value_1.bits[3] == value_2.bits[3]) {
  } else {
    ret = 0;
  }
  if (zero(value_1) && zero(value_2)) ret = 1;
  return ret;
}