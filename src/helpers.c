//
// Created by Brandon Garling on 9/21/2016.
//

#include <stdio.h>
#include <ctype.h>
#include "helpers.h"

/**
 * Skip whitespace in a file
 * @param fh - The file handle to read from
 */
void skip_whitespace(FILE *fh) {
	int c;
	do {
		c = fgetc(fh);
	}
	while(isspace(c));
	fseek(fh, -1, SEEK_CUR);
}