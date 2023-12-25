#include "s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  int err = 0;
  *result = value;
  int sign = get_bits(value, 127);
  if (sign)
    sign = 0;
  else
    sign = 1;
  set_bits(result, 127, sign);
  return err;
}
