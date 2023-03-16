#include "s21_decimal.h"

void copy_big_dec(big_dec* dest, big_dec src) {
  for (int i = LOW; i <= 6; i++) {
    dest->bits[i] = src.bits[i];
  }
}

int get_bit_big_dec(big_dec value, int index) {
  unsigned int mask = 1u;
  mask = mask << (index % MAX_BIT);
  return !!(value.bits[index / MAX_BIT] & mask);
}

void set_bit_big_dec(big_dec* value, int index, int mode) {
  unsigned int mask = 1u;
  mask = mask << (index % MAX_BIT);
  if (mode) {
    value->bits[index / MAX_BIT] = value->bits[index / MAX_BIT] | mask;
  }
  if (!mode) {
    mask = ~mask;
    value->bits[index / MAX_BIT] = value->bits[index / MAX_BIT] & mask;
  }
}

s21_bit4 get_pow_and_sign_big_dec(big_dec value) {
  s21_bit4 res = {0};
  for (int i = 0; i < 8; i++) {
    if (get_bit_big_dec(value, MAX_BIT * 6 + 16 + i)) {
      res.pow += pow(2, i);
    }
  }
  res.flag_minus = get_bit_big_dec(value, MAX_BIT * 7 - 1);
  return res;
}

void set_pow_big_dec(s21_bit4 pow_bit, big_dec* value) {
  value->bits[6] = 0;
  unsigned int mask = pow_bit.pow << 16;  // pow start from 16 bit
  value->bits[6] = value->bits[6] | mask;
  if (pow_bit.flag_minus) {
    set_bit_big_dec(value, 7 * MAX_BIT - 1, SET);
  }
}

big_dec make_big_dec_from_dec(s21_decimal value) {
  big_dec res = {0};
  res.bits[LOW] = value.bits[LOW];
  res.bits[MID] = value.bits[MID];
  res.bits[HI] = value.bits[HI];
  res.bits[6] = value.bits[POW];
  return res;
}

int move_big_dec(big_dec* value, int time) {
  int last_bit_low = 0, last_bit_mid = 0, last_bit_hi = 0, last_bit_hi1 = 0,
      last_bit_hi2 = 0;
  int error = 0;
  // print_big_dec(*value);
  for (int i = 0; i < time;  //&& (!get_bit_big_dec(*value, 6 * MAX_BIT - 1));
       i++) {
    error = i;
    last_bit_low = get_bit_big_dec(*value, MAX_BIT - 1);
    last_bit_mid = get_bit_big_dec(*value, 2 * MAX_BIT - 1);
    last_bit_hi = get_bit_big_dec(*value, 3 * MAX_BIT - 1);
    last_bit_hi1 = get_bit_big_dec(*value, 4 * MAX_BIT - 1);
    last_bit_hi2 = get_bit_big_dec(*value, 5 * MAX_BIT - 1);
    value->bits[LOW] <<= 1;
    value->bits[MID] <<= 1;
    value->bits[HI] <<= 1;
    value->bits[3] <<= 1;
    value->bits[4] <<= 1;
    value->bits[5] <<= 1;
    set_bit_big_dec(value, MAX_BIT, last_bit_low);
    set_bit_big_dec(value, 2 * MAX_BIT, last_bit_mid);
    set_bit_big_dec(value, 3 * MAX_BIT, last_bit_hi);
    set_bit_big_dec(value, 4 * MAX_BIT, last_bit_hi1);
    set_bit_big_dec(value, 5 * MAX_BIT, last_bit_hi2);
  }
  error = error == time - 1 ? 0 : 1;
  return error;
}

int s21_bit_mul(big_dec value_1, big_dec value_2, big_dec* result) {
  big_dec temp = {0};
  if (result) {
    *result = temp;
  }
  int error = 0;
  for (int i = 0; i < 192 && !error; i++) {
    if (get_bit_big_dec(value_2, i)) {
      if (s21_add_int_big_dec(temp, value_1, &temp)) {
        error = 1;
      }
      move_big_dec(&value_1, 1);
    } else {
      move_big_dec(&value_1, 1);
    }
  }
  temp.bits[6] = result->bits[6];
  *result = temp;
  return error;
}

int up_pow_big_dec(big_dec* value, int up_pow) {
  int error = 0;
  s21_bit4 pow_res = {0};
  big_dec ten = {{10, 0, 0, 0, 0, 0, 0}};
  big_dec res = {0};
  pow_res = get_pow_and_sign_big_dec(*value);
  for (int i = 0; i < up_pow; i++) {
    error = s21_bit_mul(*value, ten, &res);
    if (!error) {
      copy_big_dec(value, res);
    }
    pow_res.pow += 1;
  }
  set_pow_big_dec(pow_res, value);
  return error;
}

void normilize_big_dec(big_dec* value1, big_dec* value2) {
  s21_bit4 pow_val_1 = {0}, pow_val_2 = {0};  // pow_res = {0};
  unsigned int max_pow = 0, error = 0;
  pow_val_1 = get_pow_and_sign_big_dec(*value1);
  pow_val_2 = get_pow_and_sign_big_dec(*value2);
  max_pow = pow_val_1.pow >= pow_val_2.pow ? pow_val_1.pow : pow_val_2.pow;
  if (pow_val_1.pow != max_pow) {
    while (pow_val_1.pow != max_pow && !error) {
      up_pow_big_dec(value1, 1);
      pow_val_1 = get_pow_and_sign_big_dec(*value1);
    }
    error = 0;
  }
  if (pow_val_2.pow != max_pow) {
    while (pow_val_2.pow != max_pow && !error) {
      up_pow_big_dec(value2, 1);
      pow_val_2 = get_pow_and_sign_big_dec(*value2);
    }
    error = 0;
  }
}

void rounding(big_dec* val) {
  big_dec ten = {{10, 0, 0, 0, 0, 0, 0}};
  big_dec one = {{1, 0, 0, 0, 0, 0, 0}};
  big_dec mod = {{0, 0, 0, 0, 0, 0, 0}};
  big_dec res = {{0, 0, 0, 0, 0, 0, 0}};
  val->bits[6] = 0;
  s21_div_without_scale_big(*val, ten, val, &mod);
  if (mod.bits[0] == 5) {
    s21_div_without_scale_big(*val, ten, &res, &mod);
    if (mod.bits[0] % 2) {
      s21_add_int_big_dec(*val, one, val);
    }
  } else if (mod.bits[0] > 5) {
    s21_add_int_big_dec(*val, one, val);
  }
}

int make_dec_from_big_dec(big_dec val, s21_decimal* dest) {
  s21_bit4 pow_res = {0};
  int error = 0;
  pow_res = get_pow_and_sign_big_dec(val);
  while ((val.bits[3] || val.bits[4] || val.bits[5] || pow_res.pow > 28) &&
         !error) {
    if (!pow_res.pow) {
      if (pow_res.flag_minus) {
        error = EXTRA_SMALL_ERROR;
      } else {
        error = EXTRA_BIG_ERROR;
      }
    } else {
      rounding(&val);
      pow_res.pow--;
    }
  }
  for (int i = LOW; i < POW; i++) {
    dest->bits[i] = val.bits[i];
  }
  set_pow(pow_res, dest);
  return error;
}

// void print_binary_(unsigned int value) {
//   for (unsigned int mask = pow(2, 31); mask >= 1u; mask = mask >> 1) {
//     printf("%d", !!(value & mask));
//   }
//   printf("\n");
// }

// void print_pow_2_big_dec(big_dec val) {
//   for (int i = LEN_BIG_DEC - 1; i >= 0; i--) {
//     if (get_bit_big_dec(val, i)) {
//       printf("%d ", i);
//     }
//   }
//   printf("\n");
// }
// void print_binary_big_dec(big_dec val) {
//   printf("LOW =");
//   print_binary_(val.bits[LOW]);
//   printf("MID =");
//   print_binary_(val.bits[MID]);
//   printf("HII =");
//   print_binary_(val.bits[HI]);
//   printf("HI1 =");
//   print_binary_(val.bits[3]);
//   printf("HI2 =");
//   print_binary_(val.bits[4]);
//   printf("HI3 =");
//   print_binary_(val.bits[5]);
//   printf("POW =");
//   print_binary_(val.bits[6]);
// }
// void print_big_dec(big_dec val) {
//   s21_bit4 pow_res = get_pow_and_sign_big_dec(val);
//   printf(
//       "LOW =%u\nMID =%u\nHI =%u\nHI1 =%u\nHI2 =%u\nHI3 =%u\nPOW =%u\nPOW Mean
//       "
//       "=%u\nSign =%d\n",
//       val.bits[LOW], val.bits[MID], val.bits[HI], val.bits[3], val.bits[4],
//       val.bits[5], val.bits[6], pow_res.pow, pow_res.flag_minus);
// }