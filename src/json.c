//
// Created by Brandon Garling on 9/21/2016.
//

#include <stdio.h>
#include <string.h>
#include "json_parsers.h"
#include "json_helpers.h"
#include "json.h"

/**
 * Read a JSON file into the program into a JSONValue struct
 * @param fname - The name of the json file to load
 * @param JSONRootRef - The JSONValue struct to use for the root of this JSON file
 * @return 0 if success, otherwise a failure occurred
 */
int read_json(char* fname, JSONValue *JSONRootRef) {
	FILE *fp = fopen(fname, "r");

	// Attempt to open the input file for reading
	if (fp) {
		// Parse the JSON file!
		if (read_JSONValue(fp, JSONRootRef) != 0) {
			// Something went wrong, close the file and return an error
			fclose(fp);
			return 1;
		}
		fclose(fp);
	} else {
		fprintf(stderr, "Error: File '%s' could not be opened for reading\n", fname);
		return 1;
	}

	return 0;
}

/**
 * Resolves a JSONObject's key to a JSONValue if it exists
 * @param key - The key to look for
 * @param JSONObjectRef - The reference to the object which contains the key
 * @param JSONValueOutRef - A pointer to a pointer that will contain the JSONValue after it is resolved
 * @return 0 if success, otherwise a failure occurred
 */
int JSONObject_get_value(char* key, JSONObject* JSONObjectRef, JSONValue** JSONValueOutRef) {
	int i = 0;
	for (i = 0; i < JSONObjectRef->length; i ++) {
		if (strcmp(JSONObjectRef->keys[i], key) == 0) {
			*JSONValueOutRef = JSONObjectRef->values[i]->value;
			return 0;
		}
	}
	return 1;
}

/**
 * Resolves a JSONArray's index to a JSONValue if it exists
 * @param index - The index of the value in question
 * @param JSONArrayRef - The reference to the array which contains the value in question
 * @param JSONValueOutRef - A pointer to a pointer that will contain the JSONValue after it is resolved
 * @return 0 if success, otherwise a failure occurred
 */
int JSONArray_get_value(int index, JSONArray* JSONArrayRef, JSONValue** JSONValueOutRef) {
	if (index >= JSONArrayRef->length)
		return 1;

	*JSONValueOutRef = JSONArrayRef->values[index];
	return 0;
}