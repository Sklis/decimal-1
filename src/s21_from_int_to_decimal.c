#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  initialize(dst);
  if (src >> 31) {
    src *= -1;
    set_bits(dst, 127, 1);
  }
  dst->bits[0] = src;
  return 0;
}
