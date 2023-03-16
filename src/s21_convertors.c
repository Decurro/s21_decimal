
#include <limits.h>

#include "s21_decimal.h"
#define S21_ISNAN(X) ((X) != (X))
/// CCONVERT
int s21_from_int_to_decimal(int src, s21_decimal *dst) {  //  Из int | |
  *dst = make_dec(0, 0, 0, 0, 0);
  s21_bit4 res_pow = {0};
  res_pow.flag_minus = src >= 0 ? 0 : 1;
  if (res_pow.flag_minus) {
    src *= -1;
  }
  dst->bits[LOW] = src;
  set_pow(res_pow, dst);
  return 0;
}

void move_dot_by_pow(char *str) {
  int index = 0;
  for (int i = 0; i < (int)strlen(str); i++) {
    if (str[i] == '.') {
      index = i;
      break;
    }
  }
  for (int i = index; i < (int)strlen(str) - 1 && index != 0; i++) {
    char symbol = str[i + 1];
    str[i + 1] = str[i];
    str[i] = symbol;
  }
  if (index) str[(int)strlen(str) - 1] = 0;
}

int witch_print_use(double src, char *dst) {
  int pow = 0;
  if (src < 1) {
    sprintf(dst, "%.7lf", src);
    pow = 7;
  }
  if (src < 10 && src >= 1) {
    sprintf(dst, "%.6lf", src);
    pow = 6;
  }
  if (src >= 10 && src < 100) {
    sprintf(dst, "%.5lf", src);
    pow = 5;
  }
  if (src >= 100 && src < 1000) {
    sprintf(dst, "%.4lf", src);
    pow = 4;
  }
  if (src >= 1000 && src < 10000) {
    sprintf(dst, "%.3lf", src);
    pow = 3;
  }
  if (src >= 10000 && src < 100000) {
    sprintf(dst, "%.2lf", src);
    pow = 2;
  }
  if (src >= 100000 && src < 1000000) {
    sprintf(dst, "%.1lf", src);
    pow = 1;
  }
  if (src >= 1000000) {
    sprintf(dst, "%.0lf", src);
    pow = 0;
  }
  return pow;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int error = 0;
  if (!dst) {
    return 1;
  }
  s21_decimal zero = {{0, 0, 0, 0}};
  *dst = zero;
  if ((src < DECIMAL_MIN || src > DECIMAL_MAX ||
       (src > 0 && src < pow(10, -28))) ||
      (src < 0 && src > -pow(10, -28)) || src == -INFINITY || src == INFINITY ||
      S21_ISNAN(src)) {
    error = 1;
    dst->bits[LOW] = 0;
    dst->bits[MID] = 0;
    dst->bits[HI] = 0;
    dst->bits[POW] = 0;
  }
  if (!error) {
    double d_src = (double)src;
    s21_bit4 pow_res = {0};
    if (src < 0) {
      pow_res.flag_minus = 1;
      d_src *= -1;
    }
    char fl_to_str[256] = {0};
    pow_res.pow = witch_print_use(d_src, fl_to_str);
    move_dot_by_pow(fl_to_str);
    long long int result = 0;
    double res_d = 0;
    if (pow_res.pow != 0) {
      sscanf(fl_to_str, "%lld", &result);
      while (result % 10 == 0 && result >= 10 && pow_res.pow > 0) {
        result /= 10;
        pow_res.pow--;
      }
      dst->bits[LOW] = result;
    } else {
      sscanf(fl_to_str, "%lf", &res_d);
      while (res_d > 10000000) {
        res_d /= 10;
      }
      res_d = round(res_d);
      dst->bits[LOW] = (unsigned int)res_d;
    }
    set_pow(pow_res, dst);
  }
  return error;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {  // to int|
  int error = 0;
  s21_bit4 res_pow = {0};
  res_pow = get_pow_and_sign(src);
  s21_truncate(src, &src);
  if (src.bits[MID] || src.bits[HI]) {
    error = 1;
  } else {
    *dst = src.bits[LOW];
  }
  if (res_pow.flag_minus) {
    *dst *= -1;
  }
  return error;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int error = 0;
  if (dst) {
    *dst = 0;
  } else {
    error = 1;
  }
  if (!error) {
    big_dec src_big = make_big_dec_from_dec(src);
    big_dec ten = {{10, 0, 0, 0, 0, 0, 0}};
    big_dec mod = {0};
    big_dec zero = {0};
    s21_bit4 pow_res = get_pow_and_sign_big_dec(src_big);
    // float ff = 0;
    while (pow_res.pow) {
      s21_div_without_scale_big(src_big, ten, &src_big, &mod);
      *dst += (float)mod.bits[0];
      *dst /= 10.0f;
      pow_res.pow--;
    }
    float pow_ten = 1;
    while (!s21_is_big_equal(zero, src_big)) {
      s21_div_without_scale_big(src_big, ten, &src_big, &mod);
      *dst += (float)mod.bits[0] * pow_ten;
      pow_ten *= 10.0;
    }
    if (pow_res.flag_minus) *dst *= -1;
  }
  return error;
}

// Возвращаемое значение - код ошибки:
//  - 0 - OK
//  - 1 - ошибка конвертации
