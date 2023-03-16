// MAX DEC 79,228,162,514,264,337,593,543,950,335
// Двоичное представление Decimal состоит из 1-разрядного знака, 96-разрядного
// целого числа и коэффициента масштабирования
//Коэффициент масштабирования неявно равен числу 10, возведенному в степень в
//диапазоне от 0 до 28. Следовательно, двоичное представление Decimal имеет вид
//((от -2^96 до 2^96) / 10^(от 0 до 28))

//`bits[0]`, `bits[1]`, и `bits[2]` содержат младшие, средние и старшие 32 бита
// 96-разрядного целого числа соответственно.

/*`bits[3]` содержит коэффициент масштабирования и знак, и состоит из следующих
частей:
- Биты от 0 до 15, младшее слово, не используются и должны быть равны нулю.
- Биты с 16 по 23 должны содержать показатель степени от 0 до 28, который
указывает степень 10 для разделения целого числа.
- Биты с 24 по 30 не используются и должны быть равны нулю.
- Бит 31 содержит знак; 0 означает положительный, а 1 означает отрицательный.
*/
#ifndef S21_DECIMAL
#define S21_DECIMAL
#define MAX_BIT 32
#define MAX_BIT4_plus 917504
#define MAX_BIT4_without_pow 2147483648
#define MIN_BIT4_pow1 32768
#define MAX_INT 4294967295
#define LEN_MAX_INT 10
#define EXTRA_BIG_ERROR 1
#define EXTRA_SMALL_ERROR 2
#define Ignore_lost 1
#define Stop_if_Lost 0
#define LOST_precision 1
#define SET 1
#define CLEAN 0
#define DECIMAL_MAX (1e28 - 1)
#define DECIMAL_MIN (-1e28 + 1)
#define LEN_BIG_DEC 6 * MAX_BIT
/// 0 - OK

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
  unsigned int bits[4];
} s21_decimal;

typedef struct {
  unsigned int bits[7];
} big_dec;

typedef struct {
  unsigned int pow;
  char flag_minus;
} s21_bit4;

enum { LOW, MID, HI, POW };
// ARIFM
int s21_add(s21_decimal value_1, s21_decimal value_2,
            s21_decimal *result);  //| Сложение | + |
int s21_sub(s21_decimal value_1, s21_decimal value_2,
            s21_decimal *result);  // | Вычитание | - |
int s21_mul(s21_decimal value_1, s21_decimal value_2,
            s21_decimal *result);  // | Умножение | * |
int s21_div(s21_decimal value_1, s21_decimal value_2,
            s21_decimal *result);  // | Деление | / |
int s21_mod(s21_decimal value_1, s21_decimal value_2,
            s21_decimal *result);  // | Остаток от деления | Mod |
// Функции возвращают код ошибки:
// - 0 - OK
// - 1 - число слишком велико или равно бесконечности
// - 2 - число слишком мало или равно отрицательной бесконечности
// - 3 - деление на 0

//// COMPARE

int s21_is_less(s21_decimal, s21_decimal);  //| Меньше  | < |
int s21_is_less_or_equal(s21_decimal,
                         s21_decimal);  // | Меньше или равно | <= |
int s21_is_greater(s21_decimal, s21_decimal);  // | Больше | \> |
int s21_is_greater_or_equal(s21_decimal,
                            s21_decimal);  // | Больше или равно | \>= |
int s21_is_equal(s21_decimal, s21_decimal);      // | Равно | == |
int s21_is_not_equal(s21_decimal, s21_decimal);  // | Не равно | != |

// Возвращаемое значение:
// - 0 - FALSE
// - 1 - TRUE

/// CCONVERT
int s21_from_int_to_decimal(int src, s21_decimal *dst);      //  Из int | |
int s21_from_float_to_decimal(float src, s21_decimal *dst);  // from flot|
int s21_from_decimal_to_int(s21_decimal src, int *dst);      // to int|
int s21_from_decimal_to_float(s21_decimal src, float *dst);  // to float|

// Возвращаемое значение - код ошибки:
//  - 0 - OK
//  - 1 - ошибка конвертации

/// OTHER
int s21_floor(s21_decimal value,
              s21_decimal *result);  // | Округляет указанное Decimal число до
                                     // ближайшего целого числа в сторону
                                     // отрицательной бесконечности. |
int s21_round(
    s21_decimal value,
    s21_decimal *result);  // | Округляет Decimal до ближайшего целого числа. |
int s21_truncate(
    s21_decimal value,
    s21_decimal
        *result);  // | Возвращает целые цифры указанного Decimal числа; любые
                   // дробные цифры отбрасываются, включая конечные нули. |
int s21_negate(s21_decimal value,
               s21_decimal *result);  // | Возвращает результат умножения
                                      // указанного Decimal на -1.

// Возвращаемое значение - код ошибки:
//  - 0 - OK
//  - 1 - ошибка вычисления

/// HELP
s21_bit4 convert_bit4_to_pow(s21_decimal value);

/// @brief Convert int to str. From right to left reading
/// @param value some int
/// @return alocated str. Dont forget to free!!!
char *from_int_to_binary_str(unsigned int value);

/// @brief Insert '0' in str
/// @param str
/// @param size size of str
void make_zero_in_bit_str(char *str, int size);

/// @brief Convert binary str to int
/// @param binary_str
/// @return Unsigned int
unsigned int from_binary_str_to_int(char *binary_str);

/// @brief Convert bits[4] to integer with pow and sign
/// @param value Decimal
/// @return Struct with int pow and char sign
s21_bit4 convert_bit4_to_pow(s21_decimal value);

/// @brief Convert decimal in binary string[97]
/// @param value Decimal
/// @return Allocated string. Don't forget to free
char *decimal_to_binary_str(s21_decimal value);

/// @brief Convert decimal in long binary str with pow ten devided
/// @param value
/// @return Allocated str with binary meanig of decimal. Dont forget FREE
char *decimal_to_binary_pow(s21_decimal value);

/// @brief Moove str to right on 5 bit pow times
/// @param long_decimal
/// @param pow_bit pow of ten to moove
void move_str_to_pow(char *long_decimal, s21_bit4 pow_bit);

/// @brief Function check allocate memmory, exit if error
/// @param str pointer to str
void check_memmory_error(char *str);

/// @brief exit from program with memory error
void exit_with_error();

/// @brief Compare modul of two decimal
/// @param value1
/// @param value2
/// @return 1 if bigger or eqvial 0 if not
int s21_modul_compare_bigger_eqvial(s21_decimal value1, s21_decimal value2);

/// @brief convert dec to matissa and exp str 96 symbols both
/// Allocated 2 str, dont forget to free
/// @param value Dec
/// @param matissa NULL pointer to save res
/// @return Exponenta 96 symb
char *convert_dec_to_matissa_and_exp_str(s21_decimal value, char **matissa);

/// @brief Sum two dec str
/// @param value1
/// @param value2
/// @return Allocated result freeeeee it
char *s21_dec_str_add(const char *value1, const char *value2);

/// @brief Subb two dec str
/// @param value1
/// @param value2
/// @return Allocated result freeeeee it
char *s21_dec_str_subb(const char *value1, const char *value2);

// @brief move left , make zero in end
/// @param str
/// @param move
void moving_str(char *str, int move);

char *s21_adding(char *str_value_1, char *str_value_2);

////// NEWWW!

/************* HELP ************************/
void print_dec(s21_decimal val);
void print_binary_dec(s21_decimal val);

s21_bit4 get_pow_and_sign(s21_decimal value);
int get_bit(s21_decimal value, int index);
void set_bit(s21_decimal *value, int index, int mode);
void set_pow(s21_bit4 pow_bit, s21_decimal *value);
s21_decimal make_dec(unsigned int low, unsigned int mid, unsigned int hi,
                     unsigned int pow, int sign);
int find_last_no_zero_bit(s21_decimal value);
void copy_dec(s21_decimal *dest, s21_decimal src);
int move_dec_left(s21_decimal *value, int time);
int move_dec_right(s21_decimal *value, int time);

/************* ARIFM ************************/
int s21_add_int_dec(s21_decimal value_1, s21_decimal value_2,
                    s21_decimal *result);
int s21_sub_int_dec(s21_decimal value_1, s21_decimal value_2,
                    s21_decimal *result);
int s21_mull(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div_without_scale(s21_decimal value_1, s21_decimal value_2,
                          s21_decimal *result, s21_decimal *mod);
int div_dec_by_ten(s21_decimal value, s21_decimal *result, int time);
int mul_dec_by_ten(s21_decimal value, s21_decimal *result, int time);

s21_bit4 result_pow_bit_adding_and_decision(s21_decimal value_1,
                                            s21_decimal value_2, int *decision);
/************* OTHER ***********************/
void s21_truncate_zeros(s21_decimal *value);
int s21_truncate(s21_decimal value, s21_decimal *result);

void s21_set_scale(s21_decimal *val, int pow);
int s21_get_sign(s21_decimal val);
/************* NORMALIZE ********************/
int up_pow(s21_decimal *value);
int down_pow(s21_decimal *value);
void s21_normalize_pow(s21_decimal *value_1, s21_decimal *value_2);

/////////////// BIG DEC //////////////////
void print_big_dec(big_dec val);
void copy_big_dec(big_dec *dest, big_dec src);
void normilize_big_dec(big_dec *value1, big_dec *value2);
big_dec make_big_dec_from_dec(s21_decimal value);
int get_bit_big_dec(big_dec value, int index);
void set_bit_big_dec(big_dec *value, int index, int mode);
void set_pow_big_dec(s21_bit4 pow_bit, big_dec *value);
s21_bit4 get_pow_and_sign_big_dec(big_dec value);

int move_big_dec(big_dec *value, int time);
void move_big_dec_right(big_dec *value, int time);
void print_pow_2_big_dec(big_dec val);
int s21_add_int_big_dec(big_dec value_1, big_dec value_2, big_dec *result);
int make_dec_from_big_dec(big_dec val, s21_decimal *dest);
void s21_sub_int_big_dec(big_dec value_1, big_dec value_2, big_dec *result);

int s21_bit_mul(big_dec value_1, big_dec value_2, big_dec *result);

void s21_div_without_scale_big(big_dec value_1, big_dec value_2,
                               big_dec *result, big_dec *mod);
/// ////////// COMPARE ////////////////
int s21_is_big_less(big_dec value1, big_dec value2);
int s21_is_big_equal(big_dec vall1, big_dec vall2);
#endif  //  S21_DECIMAL