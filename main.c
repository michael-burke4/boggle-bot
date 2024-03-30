#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "words.h"

#define BOARD_SIZE 5

static const int letter_values[] = {
	1, 4, 5, 3, 1, 5, 3,
	4, 1, 7, 6, 3, 4, 2,
	1, 4, 8, 2, 2, 2, 4,
	5, 5, 7, 4, 8
};

static int get_value(char c) {
	if (c < 'a' || c > 'z')
		return -1;
	return letter_values[(int)(c - 'a')];
}


static bool is_word(char *word)
{
	ssize_t lb = 0;
	ssize_t ub = (sizeof words) / (sizeof (char *)) - 1;
	ssize_t mp;
	int cmp_res = -1;
	int count = 0;
	while (lb <= ub) {
		mp = ((lb + ub) / 2);
		cmp_res = strcmp(word, words[mp]);
		if (!cmp_res)
			return true;
		else if (cmp_res > 0)
			lb = mp + 1;
		else
			ub = mp - 1;
	}
	return false;
}

// TODO: make this a binary search that finds a match then iterates backwards
// until it doesn't match any more. Much faster that way.
static ssize_t find_first_prefix_index(char *word, size_t start) {
	size_t i = start;
	size_t len = strlen(word);
	int res;
	while ((res = strncmp(word, words[i], len)) > 0)
		++i;
	if (!res)
		return i;
	return -1;
}

typedef struct {
	char letter;
	bool double_word;
	int letter_mult;
	bool used;
} tile;

static tile board[5][5];

static void init_board(char str[BOARD_SIZE * BOARD_SIZE + 1]) {
	int str_i = 0;
	tile cur;
	for (int i = 0 ; i < BOARD_SIZE ; ++i) {
		for (int j = 0 ; j < BOARD_SIZE ; ++j) {
			board[i][j].letter = str[str_i];
			board[i][j].double_word = false;
			board[i][j].letter_mult = 1;
			board[i][j].used = false;
			str_i++;
		}
	}
}

#define RED   "\x1B[31m"
#define YEL   "\x1B[33m"
#define MAG   "\x1B[35m"
#define RESET "\x1B[0m"

static void print_board() {
	int color_flag = 0;
	for (int i = 0 ; i < BOARD_SIZE ; ++i) {
		for (int j = 0 ; j < BOARD_SIZE ; ++j) {
			if (board[i][j].letter_mult == 2) {
				printf(YEL);
				color_flag = 1;
			} else if (board[i][j].letter_mult == 3) {
				printf(RED);
				color_flag = 1;
			} else if (board[i][j].double_word) {
				printf(MAG);
				color_flag = 1;
			}
			printf("%c ", board[i][j].letter);
			if (color_flag) {
				color_flag = 0;
				printf(RESET);
			}
		}
		printf("\n");
	}
}

static tile wordstack[BOARD_SIZE * BOARD_SIZE];
static size_t stack_i = 0;

static void push(tile t) {
	if (stack_i >= BOARD_SIZE * BOARD_SIZE)
		err(1, "max word stack. Not marking tiles as unused?");
	wordstack[stack_i].letter = t.letter;
	wordstack[stack_i].double_word = t.double_word;
	wordstack[stack_i].letter_mult = t.letter_mult;
	wordstack[stack_i].used = true;
	stack_i++;
}
static void pop() {
	stack_i--;
}
static void print_stack() {
	for (size_t i = 0 ; i < stack_i ; ++i) {
		printf("%c", wordstack[i].letter);
	}
	printf("\n");
}

static int stack_value() {
	size_t i = 0;
	bool double_total = false;
	int total = 0;
	for (; i < stack_i ; ++i) {
		total += get_value(wordstack[i].letter) * wordstack[i].letter_mult;
		double_total = wordstack[i].double_word || double_total;
	}
	if (double_total)
		total *= 2;
	if (stack_i > 5)
		total += 10;
	return total;
}

static void check_stack_word(bool *is_a_word, ssize_t *first_prefix_index) {
	char word[BOARD_SIZE * BOARD_SIZE + 1];
	size_t i;
	for (i = 0 ; i < stack_i ; ++i) {
		word[i] = wordstack[i].letter;
	}
	word[i] = '\0';
	*is_a_word = is_word(word);
	*first_prefix_index = find_first_prefix_index(word, *first_prefix_index);
}

static void find_words(size_t curlen, int i, int j) {
	ssize_t prefix_index = 0;
	bool is_a_word;
	if (i < 0 || j < 0 || i >= BOARD_SIZE || j >= BOARD_SIZE || board[i][j].used)
		return;
	push(board[i][j]);
	check_stack_word(&is_a_word, &prefix_index);
	if (is_a_word) {
		printf("%d ", stack_value());
		print_stack();
	}
	if (is_a_word || prefix_index > -1) {
		board[i][j].used = 1;
		find_words(curlen + 1, i - 1, j);
		find_words(curlen + 1, i - 1, j + 1);
		find_words(curlen + 1, i, j + 1);
		find_words(curlen + 1, i + 1, j + 1);
		find_words(curlen + 1, i + 1, j);
		find_words(curlen + 1, i + 1, j - 1);
		find_words(curlen + 1, i, j - 1);
		find_words(curlen + 1, i - 1, j - 1);
	}
	board[i][j].used = 0;
	pop(board[i][j]);
}

static void find_all_words() {
	for (int i = 0 ; i < BOARD_SIZE ; ++i) {
		for (int j = 0 ; j < BOARD_SIZE ; ++j) {
			find_words(0, i, j);
		}
	}
}

static void get_row(char **row_ptr) {
	size_t len;
	size_t nread;
	while ((nread = getline(row_ptr, &len, stdin)) != (BOARD_SIZE + 1)) {
		printf("improper row size: must be %d chars long\n", BOARD_SIZE);
	}
	(*row_ptr)[5] = '\0';
}

static long read_int() {
	size_t len;
	size_t nread;
	long val;
	char *end;
	char *in = 0;
	while (1) {
		nread = getline(&in, &len, stdin);
		if (nread == -1) {
			puts("Bad input, try again");
		}
		errno = 0;
		val = strtol(in, &end, 10);
		if (errno || end == in)
			puts("Please enter a valid integer");
		else {
			free(in);
			in = 0;
			return val;
		}
	}
}

int main(void) {
	char *in = 0;
	char boardstr[BOARD_SIZE*BOARD_SIZE+1];
	boardstr[0] = 0;
	long mult;
	long mul_row;
	long mul_col;
	long dub_row;
	long dub_col;

	for (int i = 0 ; i < BOARD_SIZE ; ++i) {
		printf("Input row %d\n", i);
		get_row(&in);
		strcat(boardstr, in);
		free(in);
		in = 0;
	}
	while (1) {
		printf("What is the current letter multiplier (2 or 3)? ");
		mult = read_int();
		if (mult == 2 || mult == 3)
			break;
	}
	while (1) {
		printf("Which row is the %s letter in? ", mult == 2 ? "double" : "triple");
		mul_row = read_int();
		if (mul_row < 0 || mul_row >= BOARD_SIZE) {
			puts("Value out of range. Try again.");
			continue;
		}
		break;
	}
	while (1) {
		printf("Which col is the %s letter in? ", mult == 2 ? "double" : "triple");
		mul_col = read_int();
		if (mul_col < 0 || mul_col >= BOARD_SIZE) {
			puts("Value out of range. Try again.");
			continue;
		}
		break;
	}
	while (1) {
		printf("Which row is the double word in? (-1 if it isn't there yet) ");
		dub_row = read_int();
		if (dub_row == -1) {
			break;
		}
		if (dub_row < 0 || dub_row >= BOARD_SIZE) {
			puts("Value out of range. Try again.");
			continue;
		}
		break;
	}
	while (dub_row != -1) {
		printf("Which col is the double word in? ");
		dub_col = read_int();
		if (dub_col < 0 || dub_col >= BOARD_SIZE) {
			puts("Value out of range. Try again.");
			continue;
		}
		break;
	}

	init_board(boardstr);
	board[mul_row][mul_col].letter_mult = (unsigned int)mult;
	if (dub_row != -1)
		board[dub_row][dub_col].double_word = true;
	print_board();
}
