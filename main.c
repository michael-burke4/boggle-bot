#include <err.h>
#include <stdbool.h>
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
	return 0;
}
