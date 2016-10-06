//
// Created by Brandon Garling on 9/21/2016.
//
#include "constants.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "json_parsers.h"
#include "json.h"
#include "helpers.h"
#include "json_helpers.h"


/**
 * Read a JSONValue from a file handle.
 * @param fp - The file handle to read from
 * @param JSONValueRef - The JSONValue struct to write the found data into
 * @return 0 if success, otherwise a failure occurred
 */
int read_JSONValue(FILE *fp, JSONValue *JSONValueRef){
	int c;

	// Skip whitespace
	skip_whitespace(fp);
	// Figure out the type that we're reading
	c = getc(fp);

	fseek(fp, -1, SEEK_CUR);

	if (c == '{') {
		// An object
		JSONValueRef->type = OBJECT_T;
		// Create space for this object
		JSONValueRef->data.dataObject = malloc(sizeof(JSONObject));

		if (read_JSONObject(fp, JSONValueRef->data.dataObject) != 0) {
			return 1;
		}

		return 0;
	}
	else if (c == '[') {
		// An array
		JSONValueRef->type = ARRAY_T;
		// Create space for this array
		JSONValueRef->data.dataArray = malloc(sizeof(JSONArray));

		if (read_JSONArray(fp, JSONValueRef->data.dataArray) != 0) {
			return 1;
		}

		return 0;
	}
	else if (isdigit(c) || c == '-') {
		// An number
		JSONValueRef->type = NUMBER_T;

		// Read in the number directly
		fscanf(fp, "%f", &JSONValueRef->data.dataNumber);

		return 0;
	}
	else if (c == '"') {
		// An string
		JSONValueRef->type = STRING_T;

		// Parse the string
		char *string = parse_string(fp);
		if (string == NULL) {
			return 1;
		}

		// Set the data
		JSONValueRef->data.dataString = string;

		return 0;
	}
	else if (c == 't' || c == 'f' || c =='n') {
		// Could be 'true', 'false', 'null', or nonsense
		char string[6];
		fgets(string, 6, fp);

		if (strncmp(string, "true", 4) == 0) {
			// An true
			JSONValueRef->type = TRUE_T;

			fseek(fp, -1, SEEK_CUR);

			return 0;
		}
		if (strncmp(string, "false", 5) == 0) {
			// An false
			JSONValueRef->type = FALSE_T;

			return 0;
		}
		if (strncmp(string, "null", 4) == 0) {
			// An null
			JSONValueRef->type = NULL_T;

			fseek(fp, -1, SEEK_CUR);

			return 0;
		}

		fprintf(stderr, "Error: Found unexpected symbol '%c' when parsing for a value in a JSON file\n", c);
		return 1;
	}
	else if (c == EOF) {
		fprintf(stderr, "Error: Unexpected EOF when parsing for a value in a JSON file\n");
		return 1;
	}
	else {
		fprintf(stderr, "Error: Found unexpected symbol '%c' when parsing for a value in a JSON file\n", c);
		return 1;
	}
}

/**
 * Read a JSONObject from a file handle.
 * @param fp - The file handle to read from
 * @param JSONObjectRef - The JSONObject struct to write the found data into
 * @return 0 if success, otherwise a failure occurred
 */
int read_JSONObject(FILE *fp, JSONObject *JSONObjectRef) {
	int c;
	int size = INITIAL_BUFFER_SIZE;
	int length = 0;
	char isElementExpected = TRUE;

	JSONObjectRef->keys = malloc(sizeof(char*) * size);
	JSONObjectRef->values = malloc(sizeof(JSONElement*) * size);

	skip_whitespace(fp);

	c = getc(fp);
	if (c != '{') {
		fprintf(stderr, "Error: Found unexpected symbol '%c' when parsing for a object in a JSON file\n", c);
		return 1;
	}

	while (TRUE) {
		// Read a key value pair until we reach a '}' character
		skip_whitespace(fp);
		c = getc(fp);

		if (c == '}')
			break;
		if (c == EOF) {
			fprintf(stderr, "Error: Unexpected EOF when parsing for a value in a JSON file\n");
			return 1;
		}
		if (isElementExpected == FALSE) {
			fprintf(stderr, "Error: Elements in an object must be comma separated\n");
			return 1;
		}

		fseek(fp, -1, SEEK_CUR);

		// Make sure we have enough space for this element
		if (length == size) {
			size *= 2;
			JSONObjectRef->keys = realloc(JSONObjectRef->keys, sizeof(char*) * size);
			JSONObjectRef->values = realloc(JSONObjectRef->values, sizeof(JSONElement*) * size);
		}

		// Read the JSON element in
		JSONObjectRef->values[length] = malloc(sizeof(JSONElement));

		if (read_JSONElement(fp, JSONObjectRef->values[length]) != 0) {
			return 1;
		}

		// Set the key we found
		JSONObjectRef->keys[length] = strdup(JSONObjectRef->values[length]->key);
		length++;

		skip_whitespace(fp);

		// Expect there to be a comma if there are more elements
		c = getc(fp);

		if (c == EOF)
			isElementExpected = FALSE;
		else if (c == ',')
			isElementExpected = TRUE;
		else {
			fseek(fp, -1, SEEK_CUR);
			isElementExpected = FALSE;
		}
	}

	JSONObjectRef->length = length;

	if (LOG_LEVEL > 2) {
		print_JSONObject(JSONObjectRef);
	}

	return 0;
}

/**
 * Read a JSONElement from a file handle.
 * @param fp - The file handle to read from
 * @param JSONElementRef - The JSONElement struct to write the found data into
 * @return 0 if success, otherwise a failure occurred
 */
int read_JSONElement(FILE *fp, JSONElement *JSONElementRef) {
	int c;

	skip_whitespace(fp);
	char *key = parse_string(fp);
	skip_whitespace(fp);

	c = getc(fp);
	if (c == EOF) {
		fprintf(stderr, "Error: Unexpected EOF when parsing for an element in an object in a JSON file\n");
		return 1;
	}
	if (c != ':') {
		fprintf(stderr, "Error: Found unexpected symbol '%c' when parsing for a ':' in a JSON file\n", c);
		return 1;
	}
	skip_whitespace(fp);

	JSONElementRef->key = key;
	JSONElementRef->value = malloc(sizeof(JSONElement));

	if (read_JSONValue(fp, JSONElementRef->value) != 0) {
		return 1;
	}

	return 0;
}

/**
 * Read a JSONArray from a file handle.
 * @param fp - The file handle to read from
 * @param JSONArrayRef - The JSONArray struct to write the found data into
 * @return 0 if success, otherwise a failure occurred
 */
int read_JSONArray(FILE *fp, JSONArray *JSONArrayRef) {
	int c;
	int size = INITIAL_BUFFER_SIZE;
	int length = 0;
	char isValueExpected = TRUE;

	JSONArrayRef->values = malloc(sizeof(JSONValue*) * size);

	skip_whitespace(fp);

	c = getc(fp);
	if (c != '[') {
		fprintf(stderr, "Error: Found unexpected symbol '%c' when parsing for a object in a JSON file\n", c);
		return 1;
	}

	while (TRUE) {
		// Read a key value pair until we reach a ']' character
		skip_whitespace(fp);
		c = getc(fp);

		if (c == ']')
			break;
		if (c == EOF) {
			fprintf(stderr, "Error: Unexpected EOF when parsing for a value in a JSON file\n");
			return 1;
		}
		if (isValueExpected == FALSE) {
			fprintf(stderr, "Error: Values in an array must be comma separated\n");
			return 1;
		}

		fseek(fp, -1, SEEK_CUR);

		// Make sure we have enough space for this value
		if (length == size) {
			size *= 2;
			JSONArrayRef->values = realloc(JSONArrayRef->values, sizeof(JSONValue*) * size);
		}

		// Read the JSON element in
		JSONArrayRef->values[length] = malloc(sizeof(JSONValue));

		if (read_JSONValue(fp, JSONArrayRef->values[length]) != 0) {
			return 1;
		}

		length++;

		skip_whitespace(fp);

		// Expect there to be a comma if there are more elements
		c = getc(fp);

		if (c == EOF)
			isValueExpected = FALSE;
		else if (c == ',')
			isValueExpected = TRUE;
		else {
			fseek(fp, -1, SEEK_CUR);
			isValueExpected = FALSE;
		}
	}

	JSONArrayRef->length = length;

	if (LOG_LEVEL > 2) {
		print_JSONArray(JSONArrayRef);
	}

	return 0;
}

/**
 * Parses a simple JSON string from the file's current position,
 * this only supports simple ASCII characters.
 * @param fh - The file handle to read from
 * @return The string read from the file, or NULL if an error occurred
 */
char* parse_string(FILE *fh) {
	int c;
	int size = INITIAL_BUFFER_SIZE;
	char *buffer = malloc(sizeof(char) * size);

	// Check for a beginning quote "
	c = fgetc(fh);
	if (c != '"') {
		if (LOG_LEVEL > 0)
			fprintf(stderr, "Error: Expected string\n");
		return NULL;
	}

	// Read characters from the file until we find an ending quote "
	int i = 0;
	do {
		// Reallocate space if we don't have enough for this letter + the null terminator
		if (i + 2 > c) {
			size *= 2;
			buffer = realloc(buffer, size * sizeof(char));
		}
		c = fgetc(fh);
		buffer[i++] = c;
	}
	while (c != '"');

	// Add a null termination to the string
	buffer[--i] = '\0';

	// Make a copy of the string only the size that we need
	char* string = strdup(buffer);
	// Free our working buffer
	free(buffer);

	// Return the newly built string
	return string;
}