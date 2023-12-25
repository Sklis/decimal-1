#include "s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  initialize(result);
  *result = value;

  int sign = get_bits(value, 127);
  int exp = (unsigned int)value.bits[3] >> 16 & 0xff;
  for (int i = 0; i < exp; i++) {
    divide_by_10(result);
  }

  set_exp(result, 0);
  if (sign) {
    set_bits(result, 127, 1);
  }

  return 0;
}