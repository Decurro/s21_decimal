#include "s21_decimal.h"
#define ADDING 0
#define SUBB_2_from_1 1
#define SUBB_1_from_2 2
#define UINT_MAX 2147483647 * 2U + 1U
/// @brief Make right result POW bit and decision about adding or subbig
/// @param value_1 Dec
/// @param value_2 Dec
/// @param decision pointer of decision of add/subb
/// @return result POW bit
s21_bit4 result_pow_bit_adding_and_decision(s21_decimal value_1,
                                            s21_decimal value_2,
                                            int *decision) {
  s21_bit4 pow_value1 = {0}, pow_value2 = {0}, pow_res = {0};
  pow_value1 = get_pow_and_sign(value_1);
  pow_value2 = get_pow_and_sign(value_2);
  pow_res.pow =
      pow_value1.pow >= pow_value2.pow ? pow_value1.pow : pow_value2.pow;
  if (!pow_value1.flag_minus && !pow_value2.flag_minus) {
    pow_res.flag_minus = 0;
    *decision = ADDING;
  }
  if (pow_value1.flag_minus && pow_value2.flag_minus) {
    pow_res.flag_minus = 1;
    *decision = ADDING;
  }
  if (pow_value1.flag_minus && !pow_value2.flag_minus) {
    pow_res.flag_minus =
        s21_modul_compare_bigger_eqvial(value_1, value_2) ? 1 : 0;
    *decision = pow_res.flag_minus ? SUBB_2_from_1 : SUBB_1_from_2;
  }
  if (!pow_value1.flag_minus && pow_value2.flag_minus) {
    pow_res.flag_minus =
        s21_modul_compare_bigger_eqvial(value_1, value_2) ? 0 : 1;
    *decision = pow_res.flag_minus ? SUBB_1_from_2 : SUBB_2_from_1;
  }
  return pow_res;
}

int s21_add_int_big_dec(big_dec value_1, big_dec value_2, big_dec *result) {
  //   *result = make_dec(0, 0, 0, 0, 0);
  int val1_bit = 0, val2_bit = 0, in_mind = 0;
  big_dec zero = {{0, 0, 0, 0, 0, 0}};
  if (result) {
    *result = zero;
  }
  for (int i = 0; i < LEN_BIG_DEC; i++) {
    val1_bit = get_bit_big_dec(value_1, i);
    val2_bit = get_bit_big_dec(value_2, i);
    if (val1_bit && val2_bit) {
      if (in_mind) {
        set_bit_big_dec(result, i, SET);
      }
      in_mind = 1;
    }
    if (val1_bit ^ val2_bit) {
      if (!in_mind) {
        set_bit_big_dec(result, i, SET);
      }
    }
    if (!val1_bit && !val2_bit) {
      if (in_mind) {
        set_bit_big_dec(result, i, SET);
      }
      in_mind = 0;
    }
  }  /// Will we free result if above ???
  return in_mind;
}

/// @brief True only if 1 bigger than 2
/// @return shit
void s21_sub_int_big_dec(big_dec value_1, big_dec value_2, big_dec *result) {
  big_dec zero = {{0, 0, 0, 0, 0, 0, 0}};
  if (result) *result = zero;
  for (int i = 0; i < LEN_BIG_DEC; i++) {
    int bit1 = get_bit_big_dec(value_1, i);
    int bit2 = get_bit_big_dec(value_2, i);
    if (bit1 && !bit2) {
      set_bit_big_dec(result, i, SET);
    }
    if (!bit1 && bit2) {
      for (int k = i + 1; k < LEN_BIG_DEC && k > 0; k++) {
        if (!get_bit_big_dec(value_1, k)) {
          set_bit_big_dec(&value_1, k, SET);
        } else {
          set_bit_big_dec(&value_1, k, CLEAN);
          k = -1;
        }
      }
      set_bit_big_dec(result, i, SET);
    }
  }
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (result == NULL) {
    return 3;
  }
  int decision = 0, was_error = 0;
  s21_bit4 pow_res = {0};
  big_dec vall1 = {0}, vall2 = {0}, res = {0};
  pow_res = result_pow_bit_adding_and_decision(value_1, value_2, &decision);
  *result = make_dec(0, 0, 0, 0, 0);
  vall1 = make_big_dec_from_dec(value_1);
  vall2 = make_big_dec_from_dec(value_2);
  res = make_big_dec_from_dec(*result);
  normilize_big_dec(&vall1, &vall2);
  if (decision == ADDING) {
    s21_add_int_big_dec(vall1, vall2, &res);
  }
  if (decision == SUBB_1_from_2) {
    s21_sub_int_big_dec(vall2, vall1, &res);
  }
  if (decision == SUBB_2_from_1) {
    s21_sub_int_big_dec(vall1, vall2, &res);
  }
  set_pow_big_dec(pow_res, &res);
  was_error = make_dec_from_big_dec(res, result);
  return was_error;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2,
            s21_decimal *result) {  // | Вычитание | - |
                                    // s21_bit4 pow_res = {0};
  s21_decimal zero = {0};
  if (result) {
    *result = zero;
  }
  int error = 0;
  s21_negate(value_2, &value_2);
  error = s21_add(value_1, value_2, result);
  if (error) *result = zero;
  return error;
}

//// subb

int find_last_no_zero_bit_big(big_dec val) {
  int res = 0;
  for (int i = LEN_BIG_DEC - 1; i > 0 && !res; i--) {
    if (get_bit_big_dec(val, i)) {
      res = i;
    }
  }
  return res;
}

int is_null_big_dec(big_dec val) {
  int res = 1;
  for (int i = 0; i < 6; i++) {
    if (val.bits[i]) {
      res = 0;
    }
  }
  return res;
}

int s21_is_greater_or_equal_big_bit(big_dec value_1, big_dec value_2) {
  int result = 1;
  int f = 0;
  for (int i = 191; i >= 0 && !f; i--) {
    int a = get_bit_big_dec(value_1, i);
    int b = get_bit_big_dec(value_2, i);
    if (a != b) {
      if (a < b) result = 0;
      f = 1;
    }
  }
  return result;
}

void s21_div_without_scale_big(big_dec value_1, big_dec value_2,
                               big_dec *result, big_dec *mod) {
  big_dec zero = {0};
  *result = zero;
  *mod = zero;
  for (int i = 191; i >= 0; i--) {
    if (get_bit_big_dec(value_1, i)) set_bit_big_dec(mod, 0, 1);
    if (s21_is_greater_or_equal_big_bit(*mod, value_2)) {
      s21_sub_int_big_dec(*mod, value_2, mod);
      if (i != 0) {
        move_big_dec(mod, 1);
      }
      move_big_dec(result, 1);
      set_bit_big_dec(result, 0, 1);
    } else {
      move_big_dec(result, 1);
      if (i != 0) {
        move_big_dec(mod, 1);
      }
    }
  }
}

int s21_div_with_new_scale_big(big_dec val1, big_dec val2, big_dec *result) {
  big_dec mod = {0};
  big_dec tmp = {0};
  big_dec ten = {{10, 0, 0, 0, 0, 0, 0}};
  s21_bit4 pow_res = {0};
  int error = 0;
  s21_div_without_scale_big(val1, val2, result, &mod);
  while (!is_null_big_dec(mod) && pow_res.pow < 28 && !error) {
    s21_bit_mul(mod, ten, &mod);
    s21_bit_mul(*result, ten, result);
    pow_res.pow++;
    s21_div_without_scale_big(mod, val2, &tmp, &mod);
    s21_add_int_big_dec(*result, tmp, result);
  }
  set_pow_big_dec(pow_res, result);
  return error;
}

int s21_mul_big_dec(big_dec val1, big_dec val2, big_dec *res) {
  s21_bit4 pow_1 = {0};
  s21_bit4 pow_2 = {0};
  s21_bit4 pow_res = {0};
  big_dec time = {0};
  int error = 0;
  pow_1 = get_pow_and_sign_big_dec(val1);
  pow_2 = get_pow_and_sign_big_dec(val2);
  pow_res.pow = pow_1.pow + pow_2.pow;
  pow_res.flag_minus = pow_1.flag_minus == pow_2.flag_minus ? 0 : 1;
  if (s21_bit_mul(val1, val2, &time)) {
    error = EXTRA_BIG_ERROR;
  } else {
    set_pow_big_dec(pow_res, &time);
  }
  *res = time;
  return error;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  if (!result) {
    return EXTRA_BIG_ERROR;
  }
  s21_decimal zero = {{0, 0, 0, 0}};
  big_dec val1 = {0}, val2 = {0}, res = {0};
  *result = zero;
  int error = 0;
  val1 = make_big_dec_from_dec(value_1);
  val2 = make_big_dec_from_dec(value_2);
  if (!s21_mul_big_dec(val1, val2, &res)) {
    error = make_dec_from_big_dec(res, result);
  }
  if (error) {
    *result = zero;
  }
  return error;
}  // | Умножение | * |

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal mod = {0};
  if (result == NULL) {
    return 3;
  }
  if (s21_is_equal(value_2, mod)) {
    return 3;
  }
  int error = 0;
  *result = mod;
  big_dec val1 = {0}, val2 = {0}, res = {0};
  s21_bit4 pow_v1 = {0}, pow_v2 = {0};
  pow_v1 = get_pow_and_sign(value_1);
  pow_v2 = get_pow_and_sign(value_2);
  val1 = make_big_dec_from_dec(value_1);
  val2 = make_big_dec_from_dec(value_2);
  normilize_big_dec(&val1, &val2);
  s21_div_with_new_scale_big(val1, val2, &res);
  if (pow_v1.flag_minus != pow_v2.flag_minus) {
    s21_bit4 pow_r = {0};
    pow_r = get_pow_and_sign_big_dec(res);
    pow_r.flag_minus = 1;
    set_pow_big_dec(pow_r, &res);
  }
  error = make_dec_from_big_dec(res, result);
  if (error) *result = mod;
  return error;
}  // | Деление | / |

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_decimal mod = {0};
  if (result == NULL) {
    return 3;
  }
  if (s21_is_equal(value_2, mod)) {
    return 3;
  }
  int error = 0;
  *result = mod;
  big_dec val1 = {0}, val2 = {0}, res = {0}, modd = {0};
  s21_bit4 pow_val1 = {0}, pow_val2 = {0};
  val1 = make_big_dec_from_dec(value_1);
  val2 = make_big_dec_from_dec(value_2);
  pow_val1 = get_pow_and_sign_big_dec(val1);
  pow_val2 = get_pow_and_sign_big_dec(val2);
  normilize_big_dec(&val1, &val2);
  pow_val1.pow = pow_val1.pow > pow_val2.pow ? pow_val1.pow : pow_val2.pow;
  pow_val1.flag_minus = pow_val1.flag_minus;
  s21_div_without_scale_big(val1, val2, &res, &modd);
  set_pow_big_dec(pow_val1, &modd);
  error = make_dec_from_big_dec(modd, result);
  if (error) *result = mod;
  return error;
}  // | Деление | / |
