#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "words.h"

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
	size_t lb = 0;
	size_t ub = (sizeof words) / (sizeof (char *)) - 1;
	size_t mp;
	int cmp_res = -1;
	while (lb <= ub) {
		mp = lb + ((ub - lb) / 2);
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
	unsigned int value;
	bool double_word;
	unsigned int letter_mult;
	bool used;
} tile;

static tile board[5][5];

static void init_board(char str[26]) {
	int str_i = 0;
	tile cur;
	for (int i = 0 ; i < 5 ; ++i) {
		for (int j = 0 ; j < 5 ; ++j) {
			board[i][j].letter = str[str_i];
			board[i][j].value = get_value(str[str_i]);
			board[i][j].double_word = false;
			board[i][j].letter_mult = 1;
			board[i][j].used = false;
			str_i++;
		}
	}
}

static void print_board() {
	for (int i = 0 ; i < 5 ; ++i) {
		for (int j = 0 ; j < 5 ; ++j) {
			printf("%c ", board[i][j].letter);
		}
		printf("\n");
	}
}

int main(void) {
	init_board("onindtdwocltrrtaoehoesfen");
	print_board();
	return 0;
}
