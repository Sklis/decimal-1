#include "s21_decimal.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  initialize(result);
  s21_decimal part = {{5}};
  set_exp(&part, 1);
  int exp = get_exp(&value).exponent;
  int sign = get_sign(value);
  *result = value;

  if (exp) {
    if (sign) {
      s21_sub(value, part, result);  // отнимаем 0,5
    } else {
      s21_add(value, part, result);  // прибавляем 0.5
    }
  }
  s21_truncate(*result, result);
  return 0;
}
