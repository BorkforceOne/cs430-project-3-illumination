//
// Created by Brandon Garling on 9/21/2016.
//

#ifndef CS430_PROJECT_2_BASIC_RAYCASTER_JSON_HELPERS_H
#define CS430_PROJECT_2_BASIC_RAYCASTER_JSON_HELPERS_H

typedef struct JSONObject JSONObject;
typedef struct JSONValue JSONValue;
typedef struct JSONElement JSONElement;
typedef struct JSONArray JSONArray;

void print_JSONValue(JSONValue *JSONValueRef);
void print_JSONElement(JSONElement *JSONElementRef);
void print_JSONArray(JSONArray *JSONArrayRef);
void print_JSONObject(JSONObject *JSONObjectRef);

#endif //CS430_PROJECT_2_BASIC_RAYCASTER_JSON_HELPERS_H
