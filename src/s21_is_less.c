#include "s21_decimal.h"

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  int out = 0, flag = 0;
  int sign_1 = get_sign(value_1), sign_2 = get_sign(value_2);
  if (sign_1 == sign_2) {
    exp_compar(&value_1, &value_2);
    for (int byte = 2; byte >= 0 && flag == 0; byte--) {
      if ((unsigned)value_1.bits[byte] > (unsigned)value_2.bits[byte]) {
        if (!sign_1)
          out = 0, flag = 1;
        else
          out = 1, flag = 1;
      } else if ((unsigned)value_1.bits[byte] < (unsigned)value_2.bits[byte]) {
        if (!sign_1)
          out = 1, flag = 1;
        else
          out = 0, flag = 1;
      } else if (byte == 0 && flag == 0) {
        out = 0;
      }
    }
  } else if (!sign_1) {
    out = 0;
  } else if (sign_1) {
    out = 1;
  }
  return out;
}
