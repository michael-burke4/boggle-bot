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
	ssize_t lb = 0;
	ssize_t ub = (sizeof words) / (sizeof (char *)) - 1;
	ssize_t mp;
	int cmp_res = -1;
	int count = 0;
	while (lb <= ub) {
		mp = ((lb + ub) / 2);
		//printf("words[%ld]: %s\n", mp, words[mp]);
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

static tile wordstack[25];
static size_t stack_i = 0;

static void push(tile t) {
	if (stack_i >= 25)
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
	if (stack_i >= 5)
		total += 10;
	return total;
}

static void check_stack_word(bool *is_a_word, ssize_t *first_prefix_index) {
	char word[26];
	size_t i;
	//print_stack();
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
	if (i < 0 || j < 0 || i > 4 || j > 4 || board[i][j].used)
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
	board[i][j].used = 0; // Prob unnecessary but w/e
	pop(board[i][j]);
}

static void find_all_words() {
	for (int i = 0 ; i < 5 ; ++i) {
		for (int j = 0 ; j < 5 ; ++j) {
			find_words(0, i, j);
		}
	}
}

int main(void) {
      //init_board("qqqqqqqqqqqqqqqqqqqqqqqqq");
	init_board("aievtcleheaoqcooosndyspnb");
	board[0][0].letter_mult = 2;
	board[0][1].double_word = true;
	find_all_words();
	return 0;
}
