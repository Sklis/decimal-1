#include "s21_decimal.h"

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  initialize(result);
  if (value_2.bits[low] == 0 && value_2.bits[mid] == 0 &&
      value_2.bits[top] == 0) {
    ret = 3;  //    n / 0 = error
  } else if (value_1.bits[low] == 0 && value_1.bits[mid] == 0 &&
             value_1.bits[top] == 0) {
    for (int i = 0; i < 4; i++) result->bits[i] = 0;  //   0 / n = 0
  } else {
    ret = div_full_bits(value_1, value_2, result);
  }
  int sign = get_sign(value_1) ^ get_sign(value_2);  // Исключающее или
  set_sign(result, sign);
  if (ret == 1 && sign == 1) ret = 2;
  return ret;
}