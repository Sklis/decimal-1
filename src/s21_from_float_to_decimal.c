#include "s21_decimal.h"

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int result = CONVERTATION_OK;
  if (dst) {
    memset(dst, 0, sizeof(s21_decimal));
  }
  if (isnan(src) || src == INFINITY || src == -INFINITY) {
    result = CONVERTATION_ERROR;
    if (src == -INFINITY) {
      dst->bits[3] = -2147483648;
    }
  }
  if ((fabs(src) > 0 && fabs(src) < 1e-28) || fabs(src) >= pow(2, 96)) {
    result = CONVERTATION_ERROR;
  }
  if (dst && (result != CONVERTATION_ERROR)) {
    char *str = calloc(129, sizeof(char));
    int scale = 0;
    int sign = 0;
    if (src < 0) {
      sign = 1;
      set_sign(dst, sign);
    }
    src = fabs(src);
    ftd(src, str, &scale);
    set_scale(dst, scale);
    transferto2(&str);
    for (size_t i = 0; i < strlen(str); i++) {
      if (str[strlen(str) - 1 - i] == '1') set_bits(dst, i, sign);
    }
    free(str);
  } else {
    result = CONVERTATION_ERROR;
  }
  return result;
}
