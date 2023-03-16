//// COMPARE
#include "s21_decimal.h"

int s21_is_big_less(big_dec value1, big_dec value2) {
  int minus = 0;
  int res = 0;
  if (get_bit_big_dec(value1, 7 * MAX_BIT - 1) &&
      get_bit_big_dec(value2, 7 * MAX_BIT - 1)) {
    minus = 1;
  } else if (get_bit_big_dec(value1, 7 * MAX_BIT - 1)) {
    res = 1;
  }
  normilize_big_dec(&value1, &value2);
  for (int i = LEN_BIG_DEC - 1; i > 0 && !res; i--) {
    if (get_bit_big_dec(value1, i) > get_bit_big_dec(value2, i)) {
      res = 0;
      i = -1;
    }
    if (get_bit_big_dec(value1, i) < get_bit_big_dec(value2, i) && i > 0) {
      res = 1;
    }
  }
  if (minus) {
    res = !res;
  }
  if (!value1.bits[LOW] && !value1.bits[MID] && !value1.bits[HI] &&
      !value2.bits[LOW] && !value2.bits[MID] && !value2.bits[HI]) {
    res = 0;
  }
  return res;
}

int s21_is_less(s21_decimal value1, s21_decimal value2) {  //| Меньше  | < |
  big_dec vall1 = {0};
  big_dec vall2 = {0};
  vall1 = make_big_dec_from_dec(value1);
  vall2 = make_big_dec_from_dec(value2);
  return s21_is_big_less(vall1, vall2);
}

int s21_is_less_or_equal(s21_decimal value1,
                         s21_decimal value2) {  // | Меньше или равно | <= |
  return (s21_is_less(value1, value2) || s21_is_equal(value1, value2));
}

int s21_is_greater(s21_decimal value1, s21_decimal value2) {  // | Больше | \> |
  return (!s21_is_less(value1, value2) && !s21_is_equal(value1, value2));
}

int s21_is_greater_or_equal(s21_decimal value1,
                            s21_decimal value2) {  // | Больше или равно | \>= |
  return (!s21_is_less(value1, value2));
}

int s21_is_big_equal(big_dec vall1, big_dec vall2) {
  int res = 1;
  normilize_big_dec(&vall1, &vall2);
  for (int i = LEN_BIG_DEC - 1 + 32; i >= 0 && res; i--) {
    if (get_bit_big_dec(vall1, i) != get_bit_big_dec(vall2, i)) {
      res = 0;
    }
  }
  return res;
}

int s21_is_equal(s21_decimal value1, s21_decimal value2) {  // | Равно | == |
  big_dec vall1 = {0};
  big_dec vall2 = {0};
  int res = 0;
  if (!value1.bits[0] && !value1.bits[1] && !value1.bits[2] &&
      !value2.bits[0] && !value2.bits[1] && !value2.bits[2]) {
    res = 1;
  }
  vall1 = make_big_dec_from_dec(value1);
  vall2 = make_big_dec_from_dec(value2);

  return res ? res : s21_is_big_equal(vall1, vall2);
}

int s21_modul_compare_bigger_eqvial(s21_decimal value1, s21_decimal value2) {
  s21_decimal zero_dec = {0};
  if (s21_is_less(value1, zero_dec)) {
    s21_negate(value1, &value1);
  }
  if (s21_is_less(value2, zero_dec)) {
    s21_negate(value2, &value2);
  }
  return s21_is_greater_or_equal(value1, value2);
}

int s21_is_not_equal(s21_decimal value1,
                     s21_decimal value2) {  // | Не равно | != |
  return !s21_is_equal(value1, value2);
}
