#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  initialize(result);
  const s21_decimal one = {{1}};
  *result = value;

  int sign = get_sign(value);
  set_sign(result, sign);
  s21_truncate(*result, result);
  if (sign && get_exp(&value).exponent) s21_sub(*result, one, result);

  return 0;
}
