//
// Created by Brandon Garling on 9/22/2016.
//

#ifndef CS430_PROJECT_2_BASIC_RAYCASTER_RAYCASTER_HELPERS_H
#define CS430_PROJECT_2_BASIC_RAYCASTER_RAYCASTER_HELPERS_H

typedef struct Scene Scene;
typedef struct JSONArray JSONArray;

int JSONArray_to_V3(JSONArray *JSONArrayRef, V3 *vectorRef);
int create_scene_from_JSON(JSONValue *JSONValueSceneRef, Scene* sceneRef);

#endif //CS430_PROJECT_2_BASIC_RAYCASTER_RAYCASTER_HELPERS_H
