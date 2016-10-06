//
// Created by Brandon Garling on 9/21/2016.
//

#ifndef CS430_PROJECT_2_BASIC_RAYCASTER_PARSERS_H
#define CS430_PROJECT_2_BASIC_RAYCASTER_PARSERS_H

typedef struct JSONObject JSONObject;
typedef struct JSONValue JSONValue;
typedef struct JSONElement JSONElement;
typedef struct JSONArray JSONArray;

char* parse_string(FILE *fh);
int read_JSONValue(FILE *fp, JSONValue *JSONValueRef);
int read_JSONObject(FILE *fp, JSONObject *JSONObjectRef);
int read_JSONElement(FILE *fp, JSONElement *JSONElementRef);
int read_JSONArray(FILE *fp, JSONArray *JSONArrayRef);

#endif //CS430_PROJECT_2_BASIC_RAYCASTER_PARSERS_H
