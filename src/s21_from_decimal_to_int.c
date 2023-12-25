#include "s21_decimal.h"

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int error = CONVERTATION_ERROR;
  *dst = 0;
  s21_decimal tmp;

  if (s21_truncate(src, &tmp) == 0) {
    if ((tmp.bits[2] == 0) && (tmp.bits[1] == 0)) {
      int sign = get_sign(src);
      if (sign) {
        if (tmp.bits[0] <= (INT_MAX + 1U)) {
          *dst = tmp.bits[0];
          *dst *= -1;
          error = CONVERTATION_OK;
        }
      } else {
        if (tmp.bits[0] <= INT_MAX) {
          *dst = tmp.bits[0];
          error = CONVERTATION_OK;
        }
      }
    }
  }
  return error;
}
