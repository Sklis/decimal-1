#ifndef S21_DECIMAL_H_
#define S21_DECIMAL_H_

#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_255 255
#define s21_MAXDEC 2e96 - 1

typedef enum { CONVERTATION_OK, CONVERTATION_ERROR } ConvertorError;

typedef struct {
  unsigned int bits[4];
} s21_decimal;

// структура для представления степени
typedef struct {
  signed char exponent;
  int sign;
} expinf;

// Перечислениие для удобства {0, 1, 2, 3}
enum { low, mid, top, scale };

// Арифметические операторы
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// Операторы сравнение
int s21_is_less(s21_decimal, s21_decimal);
int s21_is_less_or_equal(s21_decimal, s21_decimal);
int s21_is_greater(s21_decimal, s21_decimal);
int s21_is_greater_or_equal(s21_decimal, s21_decimal);
int s21_is_equal(s21_decimal, s21_decimal);
int s21_is_not_equal(s21_decimal, s21_decimal);

// Преобразователи
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

// Другие функции
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

// Вспомогательные функции
int get_bits(s21_decimal value, int index);
int get_sign(s21_decimal value);
void set_bits(s21_decimal *value, int index, int sign);
void set_sign(s21_decimal *value, int sign);
void initialize(s21_decimal *result);
int zero(s21_decimal value);
expinf get_exp(s21_decimal *src);
void set_exp(s21_decimal *result, int exp);
void exp_compar(s21_decimal *value_1, s21_decimal *value_2);
void reduct_exp(s21_decimal *value_1, s21_decimal *value_2);
int div_full_bits(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result);
int signific_bits(s21_decimal dec);
int mul_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
s21_decimal div_simple(s21_decimal value_1, s21_decimal value_2,
                       s21_decimal *result);
int add_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int shift_left(s21_decimal *dec);
int shift_right(s21_decimal *dec);
int greater_or_equal_simple(s21_decimal value_1, s21_decimal value_2);
int sub_simple(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
void div_bank_round(s21_decimal *value, int flag);
int equal_simple(s21_decimal value_1, s21_decimal value_2);
int greater_simple(s21_decimal value_1, s21_decimal value_2);
int divide_by_10(s21_decimal *val);
int num_as_int(unsigned int x);
unsigned int num_as_uint(int x);
// void print_decimal(s21_decimal dec);
int getScale(s21_decimal decimal);
void transferto2(char **str);
void reverse_str(char **str);
int del_str_by_2(const char *str, char **new_str);
void ftd(float src, char *str, int *scale);
void set_scale(s21_decimal *src, int scalev);

#endif  // S21_DECIMAL_H
