//
// Created by Brandon Garling on 9/21/2016.
//

#include <stdio.h>
#include "json.h"

/**
 * Pretty print a JSONValue struct to the console
 * @param JSONValueRef - The JSONValue struct you want to print
 */
void print_JSONValue(JSONValue *JSONValueRef) {
	switch (JSONValueRef->type) {
		case STRING_T:
			printf("[JSON] Info: Value '%s' (String)\n", JSONValueRef->data.dataString);
			break;

		case NUMBER_T:
			printf("[JSON] Info: Value '%f' (Number)\n", JSONValueRef->data.dataNumber);
			break;

		case OBJECT_T:
			printf("[JSON] Info: Object\n");
			break;

		case ARRAY_T:
			printf("[JSON] Info: Array\n");
			break;

		case TRUE_T:
			printf("[JSON] Info: 'true' (Boolean)\n");
			break;

		case FALSE_T:
			printf("[JSON] Info: 'false' (Boolean)\n");
			break;

		case NULL_T:
			printf("[JSON] Info: 'null' (null)\n");
			break;
	}
}

/**
 * Pretty print a JSONElement struct to the console
 * @param JSONElementRef - The JSONElement struct you want to print
 */
void print_JSONElement(JSONElement *JSONElementRef) {
	printf("[JSON] Info: KVP '%s' : ", JSONElementRef->key);

	switch (JSONElementRef->value->type) {
		case STRING_T:
			printf("'%s' (String)\n", JSONElementRef->value->data.dataString);
			break;

		case NUMBER_T:
			printf("'%f' (Number)\n", JSONElementRef->value->data.dataNumber);
			break;

		case OBJECT_T:
			printf("(Object)\n");
			break;

		case ARRAY_T:
			printf("(Array)\n");
			break;

		case TRUE_T:
			printf("'true' (Boolean)\n");
			break;

		case FALSE_T:
			printf("'false' (Boolean)\n");
			break;

		case NULL_T:
			printf("'null' (null)\n");
			break;
	}
}

/**
 * Pretty print a JSONArray struct to the console
 * @param JSONArrayRef - The JSONArray struct you want to print
 */
void print_JSONArray(JSONArray *JSONArrayRef) {
	printf("[JSON] Info: Array with length '%d' : \n", JSONArrayRef->length);

	int i = 0;
	for (i = 0; i < JSONArrayRef->length; i++) {
		switch (JSONArrayRef->values[i]->type) {
			case STRING_T:
				printf("\t'%s' (String)\n", JSONArrayRef->values[i]->data.dataString);
				break;

			case NUMBER_T:
				printf("\t'%f' (Number)\n", JSONArrayRef->values[i]->data.dataNumber);
				break;

			case OBJECT_T:
				printf("\t(Object)\n");
				break;

			case ARRAY_T:
				printf("\t(Array)\n");
				break;

			case TRUE_T:
				printf("\t'true' (Boolean)\n");
				break;

			case FALSE_T:
				printf("\t'false' (Boolean)\n");
				break;

			case NULL_T:
				printf("\t'null' (null)\n");
				break;
		}
	}
}

/**
 * Pretty print a JSONObject struct to the console
 * @param JSONObjectRef - The JSONObject struct you want to print
 */
void print_JSONObject(JSONObject *JSONObjectRef) {
	printf("[JSON] Info: Object with '%d' keys : \n", JSONObjectRef->length);

	int i = 0;
	for (i = 0; i < JSONObjectRef->length; i++) {
		printf("\t'%s' : ", JSONObjectRef->keys[i]);

		switch (JSONObjectRef->values[i]->value->type) {
			case STRING_T:
				printf("'%s' (String)\n", JSONObjectRef->values[i]->value->data.dataString);
				break;

			case NUMBER_T:
				printf("'%f' (Number)\n", JSONObjectRef->values[i]->value->data.dataNumber);
				break;

			case OBJECT_T:
				printf("(Object)\n");
				break;

			case ARRAY_T:
				printf("(Array)\n");
				break;

			case TRUE_T:
				printf("'true' (Boolean)\n");
				break;

			case FALSE_T:
				printf("'false' (Boolean)\n");
				break;

			case NULL_T:
				printf("'null' (null)\n");
				break;
		}
	}
}