#include "s21_decimal.h"

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int ret = 0;
  initialize(result);
  int sign_res = get_sign(value_1) ^ get_sign(value_2);  // Исключающее или
  s21_decimal dec10 = {{1410065408, 2, 0, 0}};           // Число 10^10
  int exp1 = get_exp(&value_1).exponent;
  int exp2 = get_exp(&value_2).exponent;
  int exp, flag = 0;
  s21_decimal tmp;
  initialize(&tmp);
  // Формирование 3х слагаемых каждого множителя
  s21_decimal deca[3] = {0};
  s21_decimal decb[3] = {0};
  deca[2] = value_1;
  decb[2] = value_2;
  for (int i = 2; i >= 0; i--) {
    if (i > 0) {
      deca[i] = div_simple(deca[i], dec10, &deca[i - 1]);
      decb[i] = div_simple(decb[i], dec10, &decb[i - 1]);
    }
    set_exp(&deca[i], exp1);
    set_exp(&decb[i], exp2);
    exp1 -= 10;
    exp2 -= 10;
  }
  // Попарное перемножение слагаемых и суммирование
  for (int i = 2; i >= 0; i--) {
    for (int j = 2; j >= 0; j--) {
      exp1 = get_exp(&deca[i]).exponent;
      exp2 = get_exp(&decb[j]).exponent;
      ret += mul_simple(deca[i], decb[j], &tmp);
      exp = exp1 + exp2;
      set_exp(&tmp, exp);
      ret += s21_add(*result, tmp, result);
    }
  }
  // Домножение числа на 10, если экспонента больше 28
  exp = get_exp(result).exponent;
  while (exp > 28) {
    exp--;
    flag = 1;
    s21_decimal tmp = div_simple(*result, (s21_decimal){{10, 0, 0, 0}}, result);
    s21_decimal tmp_end =
        div_simple(*result, (s21_decimal){{10, 0, 0, 0}}, &tmp_end);
    if (tmp.bits[0] > 5 || (tmp.bits[0] == 5 && tmp_end.bits[0] % 2 == 1)) {
      tmp.bits[0] = 1;
      add_simple(*result, tmp, result);
    }
    set_exp(result, exp);
  }
  set_sign(result, sign_res);
  if (ret || get_exp(result).exponent < 0) {
    ret = (sign_res == 1) ? 2 : 1;
  } else if (flag && result->bits[0] == 0 && result->bits[1] == 0 &&
             result->bits[2] == 0) {
    ret = 2;
  }
  return ret;
}
