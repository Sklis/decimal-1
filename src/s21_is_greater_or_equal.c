#include "s21_decimal.h"

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int ret = 0;
  ret = s21_is_equal(value_1, value_2);
  if (ret == 0) ret = s21_is_greater(value_1, value_2);
  return ret;
}