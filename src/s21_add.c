#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int out = 0;
  initialize(result);
  exp_compar(&value_1, &value_2);
  int exp1 = get_exp(&value_1).exponent;
  int exp2 = get_exp(&value_2).exponent;
  int sign_1 = get_sign(value_1);
  int sign_2 = get_sign(value_2);
  int sign = 0;
  if (sign_1 == sign_2) {
    out = add_simple(value_1, value_2, result);
    if (sign_1 == 1) sign = 1;
  } else {
    if (greater_or_equal_simple(value_1, value_2)) {
      sub_simple(value_1, value_2, result);
      if (sign_1 == 1) sign = 1;
    } else {
      sub_simple(value_2, value_1, result);
      if (sign_1 == 0) sign = 1;
    }
  }
  set_sign(result, sign);
  set_exp(result, get_exp(&value_1).exponent);
  if (out != 0 && sign == 1) out = 2;
  if (out != 0 && exp1 && exp2) {
    div_simple(value_1, (s21_decimal){{10, 0, 0, 0}}, &value_1);
    div_simple(value_2, (s21_decimal){{10, 0, 0, 0}}, &value_2);
    set_exp(&value_1, exp1 - 1);
    set_exp(&value_2, exp2 - 1);
    out = s21_add(value_1, value_2, result);
  }
  return out;
}