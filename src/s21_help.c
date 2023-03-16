#include "s21_decimal.h"
/// @brief Exit with MEM error
void exit_with_error() {
  fprintf(stderr, "MEMORY ALLOCATE ERROR");
  exit(1);
}
/// @brief Check was that mem error or non, exit if true
/// @param str Allocated str
void check_memmory_error(char* str) {
  if (str == NULL) {
    exit_with_error();
  }
}

void copy_dec(s21_decimal* dest, s21_decimal src) {
  for (int i = LOW; i <= POW; i++) {
    dest->bits[i] = src.bits[i];
  }
}

s21_decimal make_dec(unsigned int low, unsigned int mid, unsigned int hi,
                     unsigned int pow, int sign) {
  s21_decimal res = {0};
  s21_bit4 pow_res = {0};
  pow_res.flag_minus = sign;
  pow_res.pow = pow;
  set_pow(pow_res, &res);
  res.bits[LOW] = low;
  res.bits[MID] = mid;
  res.bits[HI] = hi;
  return res;
}

int get_bit(s21_decimal value, int index) {
  unsigned int mask = 1u;
  mask = mask << (index % MAX_BIT);
  return !!(value.bits[index / MAX_BIT] & mask);
}

/// @param mode 1 = SET
/// 0 = CLEAN
void set_bit(s21_decimal* value, int index, int mode) {
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

int find_last_no_zero_bit(s21_decimal value) {
  int index = 0;
  for (int i = 3 * MAX_BIT - 1; i >= 0; i--) {
    if (get_bit(value, i)) {
      index = i;
      i = -1;
    }
  }
  return index;
}

s21_bit4 get_pow_and_sign(s21_decimal value) {
  s21_bit4 res = {0};
  for (int i = 0; i < 8; i++) {
    if (get_bit(value, MAX_BIT * 3 + 16 + i)) {
      res.pow += pow(2, i);
    }
  }
  res.flag_minus = get_bit(value, MAX_BIT * 4 - 1);
  return res;
}
///
void set_pow(s21_bit4 pow_bit, s21_decimal* value) {
  value->bits[POW] = 0;
  unsigned int mask = pow_bit.pow << 16;  // pow start from 16 bit
  value->bits[POW] = value->bits[POW] | mask;
  if (pow_bit.flag_minus) {
    set_bit(value, 4 * MAX_BIT - 1, SET);
  }
}

/// @return 1 if lost precision. 0 if not
int move_dec_left(s21_decimal* value, int time) {
  int last_bit_low = 0, last_bit_mid = 0;
  int error = 0;
  for (int i = 0; i < time && (!get_bit(*value, 3 * MAX_BIT - 1)); i++) {
    error = i;
    last_bit_low = get_bit(*value, MAX_BIT - 1);
    last_bit_mid = get_bit(*value, 2 * MAX_BIT - 1);
    value->bits[LOW] <<= 1;
    value->bits[MID] <<= 1;
    value->bits[HI] <<= 1;
    set_bit(value, MAX_BIT, last_bit_low);
    set_bit(value, 2 * MAX_BIT, last_bit_mid);
  }
  error = error == time - 1 ? 0 : 1;
  return error;
}

/// @return 1 if lost precision. 0 if no
int move_dec_right(s21_decimal* value, int time) {  //// IGNORE mode with round?
  int first_bit_hi = 0, first_bit_mid = 0;
  int error = 0;
  for (int i = 0; i < time && !get_bit(*value, 0); i++) {
    error = i;
    first_bit_hi = get_bit(*value, 2 * MAX_BIT);
    first_bit_mid = get_bit(*value, MAX_BIT);
    value->bits[LOW] >>= 1;
    value->bits[MID] >>= 1;
    value->bits[HI] >>= 1;
    set_bit(value, 2 * MAX_BIT - 1, first_bit_hi);
    set_bit(value, MAX_BIT - 1, first_bit_mid);
  }
  error = error == time - 1 ? 0 : 1;
  return error;
}

void s21_set_scale(s21_decimal* val, int pow) {
  s21_bit4 pow_res = {0};
  pow_res.pow = pow;
  set_pow(pow_res, val);
}

int s21_get_sign(s21_decimal val) { return get_bit(val, 127); }
// void print_binary_dec(s21_decimal val) {
//   printf("LOW =");
//   print_binary(val.bits[LOW]);
//   printf("MID =");
//   print_binary(val.bits[MID]);
//   printf("HII =");
//   print_binary(val.bits[HI]);
//   printf("POW =");
//   print_binary(val.bits[POW]);
//   // %u\nMID =%u\nHI =%u\nPOW =%u\n
// }
// void print_binary(unsigned int value) {
//   for (unsigned int mask = pow(2, 31); mask >= 1u; mask = mask >> 1) {
//     printf("%d", !!(value & mask));
//   }
//   printf("\n");
// }

// void print_dec(s21_decimal val) {
//   s21_bit4 pow_res = get_pow_and_sign(val);
//   printf("LOW =%u\nMID =%u\nHI =%u\nPOW =%u\nPOW Mean =%u\nSign =%d\n",
//          val.bits[LOW], val.bits[MID], val.bits[HI], val.bits[POW],
//          pow_res.pow, pow_res.flag_minus);
// }
