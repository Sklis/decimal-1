#include <math.h>

#include "s21_decimal.h"

//----------------------------------------------------------

// void print_decimal(s21_decimal dec) {
//   for (int i = 127; i >= 0; i--) {
//     if ((i == 127) || (i == 119)) printf("\033[91m");
//     if ((i > 111) && (i < 119)) printf("\033[94m");
//     if (i < 96) printf("\033[93m");
//     printf("%d\033[0m", get_bits(dec, i));
//     if (i % 32 == 0) {
//       printf(" ");
//     }
//   }
//   printf("\n");
// }

int get_sign(s21_decimal val) {
  int sign = 1;

  if ((val.bits[3] & (1 << 31)) == 0) {
    sign = 0;
  }

  return sign;
}

int get_bits(s21_decimal value, int index) {
  int res = 0;
  int number_int = index / 32;
  int index_bit = index % 32;
  res = (value.bits[number_int] & (1u << index_bit)) >> index_bit;
  return res;
}

void set_sign(s21_decimal *val, int sign) {
  if (sign == 0) {
    val->bits[3] = val->bits[3] & ~(1 << 31);
  } else {
    val->bits[3] = val->bits[3] | (1 << 31);
  }
}

void set_bits(s21_decimal *value, int index, int sign) {
  int number_int = index / 32;
  int index_bit = index % 32;
  if (sign) {
    value->bits[number_int] |= (1u << index_bit);
    // ??? получаем отрицательное число ???
  } else {
    value->bits[number_int] &= (~(1u << index_bit));
    // ??? получаем положительное число ???
  }
}

// FENIXCYC
// Чтобы избежать мусора
void initialize(s21_decimal *result) {
  result->bits[low] = 0;
  result->bits[mid] = 0;
  result->bits[top] = 0;
  result->bits[scale] = 0;
}
// Проверка на нулевое значение
int zero(s21_decimal value) {
  if (value.bits[low] == 0 && value.bits[mid] == 0 && value.bits[top] == 0) {
    return 1;
  } else {
    return 0;
  }
}
// Получение степени из decimal
expinf get_exp(s21_decimal *src) {
  expinf bits;
  int j = 0;
  for (int i = 112; i < 120; i++) {
    if (get_bits(*src, i) != 0) {
      bits.exponent = bits.exponent | (get_bits(*src, i) << j);
    } else if (get_bits(*src, i) == 0) {
      bits.exponent = bits.exponent & ~((get_bits(*src, i) + 1) << j);
    }
    j++;
  }
  bits.sign = ((src->bits[scale] >> 23) & 1);
  return bits;
}
// Запись степени в decimal
void set_exp(s21_decimal *result, int exp) {
  int i = 112;
  int k = 0;
  while (i < 120) {
    set_bits(result, i, (exp & (1u << k)) >> k);
    i++;
    k++;
  }
}

int div_full_bits(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result) {
  int ret = 0;
  int err = 0;
  int num_bits_2 = signific_bits(*result);
  int exp = get_exp(&value_1).exponent - get_exp(&value_2).exponent - 1;
  s21_decimal tmp = {{0, 0, 0, 0}};
  // Выход если нет остатка от деления или заполнены все биты в result
  while ((num_bits_2 < 96) && exp < 28 && !err &&
         (value_1.bits[low] != 0 || value_1.bits[mid] != 0 ||
          value_1.bits[top] != 0)) {
    err = mul_simple(*result, (s21_decimal){{10, 0, 0, 0}}, result);
    if (!err) {
      exp++;
      value_1 = div_simple(value_1, value_2, &tmp);  // остаток
      while (mul_simple(value_1, (s21_decimal){{10, 0, 0, 0}}, &value_1)) {
        // остаток нельзя mul на 10
        div_simple(value_1, (s21_decimal){{2, 0, 0, 0}}, &value_1);
        div_simple(value_2, (s21_decimal){{2, 0, 0, 0}}, &value_2);
      }
      num_bits_2 = signific_bits(*result);
    }
  }
  // bankround к ближайшему четному
  s21_decimal tmp_end = {{0, 0, 0, 0}};
  div_simple(value_1, value_2, &tmp_end);
  if (tmp_end.bits[0] > 5 || (tmp_end.bits[0] == 5 && tmp.bits[0] % 2 == 1)) {
    tmp_end.bits[0] = 1;
    add_simple(*result, tmp_end, result);
  }
  // reduct экспоненты
  err = 0;
  while (exp < 0 && !err) {
    exp++;
    err = mul_simple(*result, (s21_decimal){{10, 0, 0, 0}}, result);
  }
  set_exp(result, exp);
  if (result->bits[0] == 0 && result->bits[1] == 0 && result->bits[2] == 0) {
    ret = 2;
  } else if (err) {
    ret = 1;
  }
  return ret;
}

void exp_compar(s21_decimal *value_1, s21_decimal *value_2) {
  int exp_1 = get_exp(value_1).exponent;
  int exp_2 = get_exp(value_2).exponent;
  if (exp_1 > exp_2) {
    reduct_exp(value_1, value_2);
  } else if (exp_1 < exp_2) {
    reduct_exp(value_2, value_1);
  }
}

// exp value_1 > exp value_2
void reduct_exp(s21_decimal *value_1, s21_decimal *value_2) {
  int exp_1 = get_exp(value_1).exponent, exp_2 = get_exp(value_2).exponent;
  int diff = exp_1 - exp_2;
  while (diff > 0 &&
         !mul_simple(*value_2, (s21_decimal){{10, 0, 0, 0}}, value_2)) {
    diff--;
    exp_2++;
  }
  int flag = 0;
  while (diff > 0) {
    if (diff == 1) {
      div_bank_round(value_1, flag);
    } else {
      s21_decimal tmp =
          div_simple(*value_1, (s21_decimal){{10, 0, 0, 0}}, value_1);
      if (tmp.bits[0]) flag = 1;
    }
    diff--;
    exp_1--;
  }
  set_exp(value_1, exp_1);
  set_exp(value_2, exp_2);
}

int signific_bits(s21_decimal dec) {
  int count = 96;
  s21_decimal tmp = dec;
  while (!shift_left(&tmp) && count > 0) {
    count--;
  }
  return count;
}
int mul_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal tmp = *result;
  int flag = 0;
  for (int i = 0; i < 3; i++) result->bits[i] = 0;
  // Кол-во значащих цифр во втором множителе
  int count = signific_bits(value_2);
  //  Умножение
  for (int i = 0; i < count && flag == 0; i++) {
    if (get_bits(value_2, i) == 1) {
      flag = add_simple(*result, value_1, result);
    }
    if (shift_left(&value_1) && i != count - 1) flag = 1;
  }
  if (flag) *result = tmp;
  return flag;  // 1 - ошибка переполнения
}
s21_decimal div_simple(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *result) {
  for (int i = 0; i < 3; i++) result->bits[i] = 0;
  int num_bits_1 = signific_bits(value_1);
  int num_bits_2 = signific_bits(value_2);
  // Выравниваем биты второго числа по левому краю первого числа
  int diff = num_bits_1 - num_bits_2;
  for (int i = 0; i < diff; i++) {
    shift_left(&value_2);
  }
  // Деление
  for (int i = 0; i <= diff && !shift_left(result); i++) {
    if (greater_or_equal_simple(value_1, value_2)) {
      sub_simple(value_1, value_2, &value_1);
      result->bits[0] += 1;
    }
    shift_right(&value_2);
  }
  return value_1;
}

int add_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag = 0;
  for (int bit_num = 0; bit_num < 96; bit_num++) {
    int bit1 = get_bits(value_1, bit_num);
    int bit2 = get_bits(value_2, bit_num);
    int bit3 = bit1 + bit2 + flag;
    flag = bit3 / 2;
    set_bits(result, bit_num, bit3 % 2);
  }
  return flag;  // 0 - OK; 1 - inf
}

int shift_left(s21_decimal *dec) {
  int flag = (dec->bits[2] & (1 << 31))
                 ? 1
                 : 0;  // 1 - будет переполнение сдвиг невозможен
  if (!flag) {
    for (int byte = 0; byte < 3; byte++) {
      int flagn = (dec->bits[byte] & (1 << 31)) ? 1 : 0;
      dec->bits[byte] <<= 1;
      dec->bits[byte] += flag;
      flag = flagn;
    }
  }
  return flag;
}

int shift_right(s21_decimal *dec) {
  int flag =
      (dec->bits[0] & 1) ? 1 : 0;  // 1 - будет переполнение сдвиг невозможен
  if (!flag) {
    for (int byte = 2; byte >= 0; byte--) {
      int flagn = (dec->bits[byte] & 1) ? 1 : 0;
      dec->bits[byte] >>= 1;
      set_bits(dec, 32 * byte + 31, flag);
      flag = flagn;
    }
  }
  return flag;
}

int greater_or_equal_simple(s21_decimal value_1, s21_decimal value_2) {
  int flag = equal_simple(value_1, value_2);
  flag += greater_simple(value_1, value_2);
  flag = (flag == 0) ? 0 : 1;
  return flag;
}

int sub_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int flag = 0;
  for (int bit_num = 0; bit_num < 96; bit_num++) {
    int bit1 = get_bits(value_1, bit_num);
    int bit2 = get_bits(value_2, bit_num);
    int bit3 = bit1 - bit2 - flag;
    set_bits(result, bit_num, (bit3 + 2) % 2);
    flag = (bit3 < 0) ? 1 : 0;
  }
  return flag;
}

void div_bank_round(s21_decimal *value, int flag) {
  s21_decimal trash;
  s21_decimal tmp = div_simple(*value, (s21_decimal){{10, 0, 0, 0}}, value);
  s21_decimal tmp_end =
      div_simple(*value, (s21_decimal){{10, 0, 0, 0}}, &trash);
  if (tmp.bits[0] > 5 || (tmp.bits[0] == 5 && flag) ||
      (tmp.bits[0] == 5 && tmp_end.bits[0] % 2 == 1)) {
    tmp.bits[0] = 1;
    add_simple(*value, tmp, value);
  }
}

int equal_simple(s21_decimal value_1, s21_decimal value_2) {
  int flag = 1;
  for (int byte = 2; byte >= 0 && flag == 1; byte--) {
    if ((unsigned)value_1.bits[byte] != (unsigned)value_2.bits[byte]) {
      flag = 0;
    }
  }
  return flag;
}

int greater_simple(s21_decimal value_1, s21_decimal value_2) {
  int flag = 0;
  for (int byte = 2; byte >= 0 && flag == 0; byte--) {
    if ((unsigned)value_1.bits[byte] > (unsigned)value_2.bits[byte]) {
      flag = 1;
    }
    if ((unsigned)value_1.bits[byte] < (unsigned)value_2.bits[byte]) {
      flag = -1;
    }
  }
  flag = (flag == 1) ? 1 : 0;
  return flag;
}

int divide_by_10(s21_decimal *val) {
  unsigned long long num, div;

  num = num_as_uint(val->bits[2]);
  div = num % 10;
  val->bits[2] = num_as_int((unsigned int)(num / 10));

  num = num_as_uint(val->bits[1]) +
        div * (4294967296);  // 4294967296- число в двоичной системе:
                             // 100000000000000000000000000000000, 2^32
  div = num % 10;
  val->bits[1] = num_as_int((unsigned int)(num / 10));

  num = num_as_uint(val->bits[0]) + div * (4294967296);
  div = num % 10;
  val->bits[0] = num_as_int((unsigned int)(num / 10));

  return (int)div;
}

unsigned int num_as_uint(int x) { return *((unsigned int *)&x); }

int num_as_int(unsigned int x) { return *((int *)&x); }

int getScale(const s21_decimal decimal) {
  int mask = 127 << 16;
  int scale = (mask & decimal.bits[3]) >> 16;
  return scale;
}

void set_scale(s21_decimal *src, int scale) {
  src->bits[3] = (get_bits(*src, 127) << 31) | (scale << 16);
}

void ftd(float src, char *str, int *scale) {
  sprintf(str, "%e", src);
  char *e = strchr(str, 'e');
  *scale += (*(e + 2) - '0') * 10;
  *scale += *(e + 3) - '0';
  if (*(e + 1) == '-') {
    *scale *= -1;
  }
  *e = '\0';
  int temp = 6;
  char *end = str + strlen(str) - 1;
  while (*end != '.' && *end == '0') {
    temp -= 1;
    end--;
  }
  *scale -= temp;
  if (*end == '.') {
    *end = '\0';
  } else {
    *(end + 1) = '\0';
    memmove(str + 1, str + 2, strlen(str + 2));
    *end = '\0';
  }
  while (*scale > 0) {
    end = str + strlen(str);
    *end = '0';
    *(end + 1) = '\0';
    (*scale)--;
  }
  if (*scale < 0) {
    *scale *= -1;
  }
}

int del_str_by_2(const char *str, char **new_str) {
  free(*new_str);
  *new_str = calloc(128, sizeof(char));
  char *current = *new_str;
  int ost = (*(str + strlen(str) - 1) - '0') % 2;
  int d = 0, opers = 0;
  while (*str) {
    opers++;
    d = d * 10 + (*str - '0');
    str++;
    while (d < 2 && *str) {
      if (opers != 1) {
        *current = '0';
        current++;
      }
      d *= 10;
      d += *str - '0';
      str++;
      opers++;
    }
    *current = (d / 2) + '0';
    current++;
    d -= (d / 2) * 2;
  }
  return ost;
}

void reverse_str(char **str) {
  char *new = calloc(strlen(*str) + 1, sizeof(char));
  for (size_t i = 0; i < strlen(*str); i++)
    new[i] = (*str)[strlen(*str) - i - 1];
  free(*str);
  *str = new;
}

void transferto2(char **str) {
  char *new = NULL;
  int res = del_str_by_2(*str, &new);
  char *digit = calloc(129, sizeof(char)), *c = digit;
  while ((*new != '1' && *new != '0') || strlen(*str) > 1) {
    *(c++) = res + '0';
    *str = strcpy(*str, new);
    res = del_str_by_2(*str, &new);
  }
  *(c++) = res + '0';
  *c = *new;
  reverse_str(&digit);
  free(*str);
  *str = digit;
  free(new);
}
