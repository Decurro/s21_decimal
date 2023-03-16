cc = gcc -Wall -Werror -Wextra -std=c11
# cc = gcc
GCOVFLAGS = -fprofile-arcs -ftest-coverage
BREAKPOINT = b *.c:12
OS = $(shell uname)
ifeq ($(OS), Linux)
 CHECK_FLAGS = -lcheck -pthread -lrt -lm -lsubunit
else
 CHECK_FLAGS = -lcheck
endif

all: s21_decimal.a

s21_decimal.a:
	$(cc) -c *.c
	ar rcs s21_decimal.a *.o
	rm -rf *.o
	ranlib s21_decimal.a

test: clean s21_decimal.a
	$(cc) s21_decimal.a 1/*.c $(CHECK_FLAGS) -o s21_test -L.
	./s21_test

gcov: clean
	gcc $(GCOVFLAGS) *.c 1/*.c $(CHECK_FLAGS) -o unit_test
	./unit_test
	lcov -t "./unit_test"  -o report.info --no-external -c -d .
	genhtml -o report report.info
	open ./report/index.html

	rm -rf *.gcno *gcda *.gco

debug:
	gcc -g *.c
	lldb a.out
	$(BREAKPOINT)
	run
clang:
	cp ../materials/linters/.clang-format .clang-format
	clang-format -n *.h
	clang-format -n ./1/*.c *.c

leak: test
	CK_FORK=no leaks --atExit -- ./s21_test

valgrind: test
	valgrind --trace-children=yes --track-fds=yes --track-origins=yes --leak-check=full --show-leak-kinds=all ./s21_test

clean:
	rm -rf *.o *.gcno *.gcda *.a *.info report .clang-format s21_test *.out unit_test

###
