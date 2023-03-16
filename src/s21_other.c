/// OTHER
#include "s21_decimal.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  int error = 0;
  if (result != NULL) {
    *result = value;
    s21_decimal one = {{1, 0, 0, 0}};
    s21_decimal five = {{5, 0, 0, 32768 * 2}};
    s21_decimal work_1 = {{0}};
    s21_decimal work_2 = {{0}};
    s21_decimal work_3 = {{0}};

    s21_bit4 new = {0};
    new = get_pow_and_sign(value);
    new.flag_minus = 0;
    set_pow(new, &value);

    s21_truncate(value, &work_1);
    s21_sub(value, work_1, &work_2);
    *result = work_1;
    if (s21_is_greater_or_equal(work_2, five) == 1) {
      s21_add(work_1, one, &work_3);
      *result = work_3;
    }
  } else {
    error = 1;
  }
  return error;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int error = 0;
  if (result != NULL) {
    *result = value;
    s21_decimal ten = {{10, 0, 0, 0}};
    s21_decimal modd = {{0, 0, 0, 0}};
    s21_bit4 new = {0};
    new = get_pow_and_sign(value);
    char sign = new.flag_minus;
    int pow = new.pow;

    if (!pow) {
      *result = value;
    } else {
      for (int i = pow; i > 0; i--) {
        s21_div_without_scale(value, ten, result, &modd);
        value = *result;
      }
    }
    new.pow = 0;
    new.flag_minus = sign;
    set_pow(new, result);
  } else {
    error = 1;
  }

  return error;
}

int s21_floor(s21_decimal value, s21_decimal *result) {
  int error = 0;
  if (result != NULL) {
    *result = value;
    s21_decimal one = {{1, 0, 0, 2147483648}};
    s21_decimal res = {{0}};
    s21_decimal check = {0};
    check = value;
    s21_bit4 new = {0};
    new = get_pow_and_sign(value);
    s21_truncate(value, &value);
    int uverennost = s21_is_equal(check, value);
    *result = value;
    if (new.flag_minus == 1 && new.pow != 0 && uverennost != 1) {
      s21_add(value, one, &res);
      *result = res;
    }
  } else {
    error = 1;
  }
  return error;
}

int s21_negate(s21_decimal value,
               s21_decimal *result) {  // | Возвращает результат умножения
                                       // указанного Decimal на -1.
  *result = value;
  if (get_bit(value, 127)) {
    set_bit(result, 127, 0);
  } else {
    set_bit(result, 127, 1);
  }
  return 0;
}
