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

static ssize_t find_first_prefix_index(char *word) {
	size_t i = 0;
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

tile build_tile(char letter) {
	tile ret = {
		.letter = letter,
		.value = get_value(letter),
		.double_word = false,
		.letter_mult = 1,
		.used = false,
	};
	return ret;
}

int main(void) {
	printf("%ld\n", find_first_prefix_index("syz"));
	return 0;
}
