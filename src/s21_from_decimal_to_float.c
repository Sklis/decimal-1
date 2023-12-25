#include "s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  double temp = (double)*dst;
  for (int i = 0; i < 96; i++) {
    temp += get_bits(src, i) * pow(2, i);
  }
  temp = temp * pow(10, -getScale(src));
  if (get_bits(src, 127)) temp = temp * (-1);
  *dst = temp;
  return 0;
}
