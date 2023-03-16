#include "s21_decimal.h"
#define ADDING 0
#define SUBB_2_from_1 1
#define SUBB_1_from_2 2

int s21_add_int_dec(s21_decimal value_1, s21_decimal value_2,
                    s21_decimal *result) {
  *result = make_dec(0, 0, 0, 0, 0);
  int val1_bit = 0, val2_bit = 0, in_mind = 0;
  for (int i = 0; i < 3 * MAX_BIT; i++) {
    val1_bit = get_bit(value_1, i);
    val2_bit = get_bit(value_2, i);
    if (val1_bit && val2_bit) {
      if (in_mind) {
        set_bit(result, i, SET);
      }
      in_mind = 1;
    }
    if (val1_bit ^ val2_bit) {
      if (!in_mind) {
        set_bit(result, i, SET);
      }
    }
    if (!val1_bit && !val2_bit) {
      if (in_mind) {
        set_bit(result, i, SET);
      }
      in_mind = 0;
    }
  }  /// Will we free result if above ???
  return in_mind;
}

int s21_sub_int_dec(s21_decimal value_1, s21_decimal value_2,
                    s21_decimal *result) {
  *result = make_dec(0, 0, 0, 0, 0);
  value_1.bits[POW] = 0;
  value_2.bits[POW] = 0;
  s21_decimal one = {0};
  s21_decimal tmp = {0};
  one.bits[LOW] = 1;
  for (int i = LOW; i < POW; i++) {
    value_2.bits[i] = ~value_2.bits[i];
  }
  s21_add_int_dec(value_2, one, &tmp);
  copy_dec(&value_2, tmp);
  s21_add_int_dec(value_1, value_2, result);
  // set_bit(result, find_last_no_zero_bit(*result), CLEAN);
  // //???????????????????????????
  return 0;  /// FIX IT
}

int s21_div_without_scale(s21_decimal value_1, s21_decimal value_2,
                          s21_decimal *result, s21_decimal *mod) {
  int error = 0, equal = 0;
  value_1.bits[POW] = 0;
  value_2.bits[POW] = 0;
  s21_decimal tmp_res_sub = {0}, tmp_res_div = {0}, one_dec = {{1, 0, 0, 0}},
              zero_dec = {{0, 0, 0, 0}};
  if (s21_is_equal(value_2, zero_dec)) {
    error = 3;
  }
  if (s21_is_equal(value_1, value_2)) {
    equal = 1;
  }
  if (!error && s21_is_greater(value_1, value_2)) {
    int count_of_div_numb =
        find_last_no_zero_bit(value_1) - find_last_no_zero_bit(value_2) + 1;
    int flag_first_in = 0;
    move_dec_left(&value_2, count_of_div_numb - 1);
    while (count_of_div_numb > 0) {
      if (s21_is_greater_or_equal(value_1, value_2)) {
        s21_sub_int_dec(value_1, value_2, &tmp_res_sub);
        copy_dec(&value_1, tmp_res_sub);
        if (!flag_first_in) {
          copy_dec(&tmp_res_div, one_dec);
        } else {
          move_dec_left(&tmp_res_div, 1);
          set_bit(&tmp_res_div, 0, SET);
        }
      } else {
        if (!flag_first_in) {
          copy_dec(&tmp_res_div, zero_dec);
        } else {
          move_dec_left(&tmp_res_div, 1);
        }
      }
      move_dec_right(&value_2, 1);
      flag_first_in = 1;
      count_of_div_numb--;
    }
    if (mod) copy_dec(mod, value_1);
  }
  equal ? copy_dec(result, one_dec) : copy_dec(result, tmp_res_div);
  return error;
}
// int s21_mull(s21_decimal value_1, s21_decimal value_2,
//              s21_decimal *result) {  /// TRUNCATE and work with scale. if we
//                                      /// know that scale no zero we will
//                                      check
//                                      /// truncate in iterration
//   s21_decimal res = {0}, tmp = {0};
//   int val2_bit = 0, error = 0;
//   for (int i = 0; i < 3 * MAX_BIT && !error; i++) {
//     copy_dec(&tmp, value_1);
//     copy_dec(result, res);
//     val2_bit = get_bit(value_2, i);
//     if (val2_bit) {
//       for (int k = 1; k <= i && !error; k++) {
//         error = move_dec_left(&tmp, 1);
//       }
//       if (!error) {
//         res = make_dec(0, 0, 0, 0, 0);
//         error = s21_add_int_dec(*result, tmp, &res);
//       }
//     }
//   }
//   set_res_pow_and_sign_mul(value_1, value_2, &res, 0);
//   copy_dec(result, res);
//   return error;
// }
