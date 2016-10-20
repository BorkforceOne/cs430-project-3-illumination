//
// Created by Brandon Garling on 9/22/2016.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "3dmath.h"
#include "raycaster.h"

/**
 * Converts a JSONArray to a V3 vector with error checking
 * @param JSONArrayRef - A reference to an JSONArray which contains 3 number elements
 * @param vectorRef - The vector to write into
 * @return 0 if success, otherwise a failure occurred
 */
int JSONArray_to_V3(JSONArray *JSONArrayRef, V3 *vectorRef) {
	JSONValue *JSONValueTempRef;

	if (JSONArrayRef->length != 3){
		fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
		return 1;
	}

	int i = 0;
	for (i = 0; i < 3; i ++) {
		if (JSONArray_get_value(i, JSONArrayRef, &JSONValueTempRef) != 0) {
			fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
			return 1;
		}

		if (JSONValueTempRef->type != NUMBER_T) {
			fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
			return 1;
		}

		vectorRef->array[i] = JSONValueTempRef->data.dataNumber;
	}

	return 0;
}

/**
 * Populates a scene based on the input JSONRootValue
 * @param JSONValueSceneRef - The JSON value containing a JSONArray to be used to populate the scene
 * @param sceneRef - A reference to the scene to populate
 * @return 0 if success, otherwise a failure occurred
 */
int create_scene_from_JSON(JSONValue *JSONValueSceneRef, Scene* sceneRef) {
	JSONObject *JSONObjectTempRef;
	JSONValue *JSONValueTempRef;
	JSONArray *JSONSceneArrayRef;

	// Make sure that we were passed a JSONArray
	if (JSONValueSceneRef->type != ARRAY_T) {
		fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
		return 1;
	}
	JSONSceneArrayRef = JSONValueSceneRef->data.dataArray;

	int size = JSONSceneArrayRef->length - 1;
	sceneRef->primitives = malloc(sizeof(Primitive*) * size);
	sceneRef->lights = malloc(sizeof(Light*) * size);

	int primitivesLength = 0;
	int lightsLength = 0;
	for (int i = 0; i < JSONSceneArrayRef->length; i++) {
		// Look at the objects we loaded in JSON
		if (JSONSceneArrayRef->values[i]->type == OBJECT_T) {
			// Everything should have a type
			JSONObjectTempRef = JSONSceneArrayRef->values[i]->data.dataObject;

			if (JSONObject_get_value("type", JSONObjectTempRef, &JSONValueTempRef) != 0) {
				fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
				return 1;
			}
			if (JSONValueTempRef->type != STRING_T) {
				fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
				return 1;
			}


			if (strcmp(JSONValueTempRef->data.dataString, "camera") == 0) {
				// We found a camera

				// Read the height
				if (JSONObject_get_value("height", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != NUMBER_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->data.dataNumber < 0) {
					fprintf(stderr, "Error: Negative camera height is not allowed\n");
					return 1;
				}
				sceneRef->camera.height = JSONValueTempRef->data.dataNumber;

				// Read the width

				if (JSONObject_get_value("width", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != NUMBER_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->data.dataNumber < 0) {
					fprintf(stderr, "Error: Negative camera width is not allowed\n");
					return 1;
				}

				sceneRef->camera.width = JSONValueTempRef->data.dataNumber;
			}
			else if (strcmp(JSONValueTempRef->data.dataString, "sphere") == 0) {
				// We found a sphere
				sceneRef->primitives[primitivesLength] = malloc(sizeof(Primitive));
				sceneRef->primitives[primitivesLength]->type = SPHERE_T;

				// Read the diffuse color
				if (JSONObject_get_value("diffuse_color", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != ARRAY_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}

				if (JSONArray_to_V3(JSONValueTempRef->data.dataArray, &sceneRef->primitives[primitivesLength]->data.sphere.diffuseColor) != 0) {
					return 1;
				}

				// Check colors
				for (int j = 0; j < 3; j++) {
					if (sceneRef->primitives[primitivesLength]->data.sphere.diffuseColor.array[j] < 0) {
						fprintf(stderr, "Error: Color cannot be negative\n");
						return 1;
					}
					if (sceneRef->primitives[primitivesLength]->data.sphere.diffuseColor.array[j] > 1) {
						fprintf(stderr, "Error: Primitive colors cannot be greater than 1.0\n");
						return 1;
					}
				}

				// Read the specular color
				if (JSONObject_get_value("specular_color", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != ARRAY_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}

				if (JSONArray_to_V3(JSONValueTempRef->data.dataArray, &sceneRef->primitives[primitivesLength]->data.sphere.specularColor) != 0) {
					return 1;
				}

				// Check colors
				for (int j = 0; j < 3; j++) {
					if (sceneRef->primitives[primitivesLength]->data.sphere.specularColor.array[j] < 0) {
						fprintf(stderr, "Error: Color cannot be negative\n");
						return 1;
					}
					if (sceneRef->primitives[primitivesLength]->data.sphere.specularColor.array[j] > 1) {
						fprintf(stderr, "Error: Primitive colors cannot be greater than 1.0\n");
						return 1;
					}
				}

				// Read the position
				if (JSONObject_get_value("position", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != ARRAY_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}

				if (JSONArray_to_V3(JSONValueTempRef->data.dataArray, &sceneRef->primitives[primitivesLength]->data.sphere.position) != 0) {
					return 1;
				}

				// Read the radius
				if (JSONObject_get_value("radius", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != NUMBER_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->data.dataNumber < 0) {
					fprintf(stderr, "Error: Negative sphere radius is not allowed\n");
					return 1;
				}

				sceneRef->primitives[primitivesLength]->data.sphere.radius = JSONValueTempRef->data.dataNumber;
				primitivesLength++;
			}
			else if (strcmp(JSONValueTempRef->data.dataString, "plane") == 0) {
				// We found a plane
				sceneRef->primitives[primitivesLength] = malloc(sizeof(Primitive));
				sceneRef->primitives[primitivesLength]->type = PLANE_T;

				// Read the diffuse color
				if (JSONObject_get_value("diffuse_color", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != ARRAY_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}

				if (JSONArray_to_V3(JSONValueTempRef->data.dataArray, &sceneRef->primitives[primitivesLength]->data.plane.diffuseColor) != 0) {
					return 1;
				}

				// Check colors
				for (int j = 0; j < 3; j++) {
					if (sceneRef->primitives[primitivesLength]->data.plane.diffuseColor.array[j] < 0) {
						fprintf(stderr, "Error: Color cannot be negative\n");
						return 1;
					}
					if (sceneRef->primitives[primitivesLength]->data.plane.diffuseColor.array[j] > 1) {
						fprintf(stderr, "Error: Primitive colors cannot be greater than 1.0\n");
						return 1;
					}
				}

				// Read the specular color
				if (JSONObject_get_value("specular_color", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != ARRAY_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}

				if (JSONArray_to_V3(JSONValueTempRef->data.dataArray, &sceneRef->primitives[primitivesLength]->data.plane.specularColor) != 0) {
					return 1;
				}

				// Check colors
				for (int j = 0; j < 3; j++) {
					if (sceneRef->primitives[primitivesLength]->data.plane.specularColor.array[j] < 0) {
						fprintf(stderr, "Error: Color cannot be negative\n");
						return 1;
					}
					if (sceneRef->primitives[primitivesLength]->data.plane.specularColor.array[j] > 1) {
						fprintf(stderr, "Error: Primitive colors cannot be greater than 1.0\n");
						return 1;
					}
				}

				// Read the position
				if (JSONObject_get_value("position", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != ARRAY_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}

				if (JSONArray_to_V3(JSONValueTempRef->data.dataArray, &sceneRef->primitives[primitivesLength]->data.plane.position) != 0) {
					return 1;
				}

				// Read the normal
				if (JSONObject_get_value("normal", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != ARRAY_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}

				if (JSONArray_to_V3(JSONValueTempRef->data.dataArray, &sceneRef->primitives[primitivesLength]->data.plane.normal) != 0) {
					return 1;
				}

				// Normalize the direction
				v3_normalize(&sceneRef->primitives[primitivesLength]->data.plane.normal, &sceneRef->primitives[primitivesLength]->data.plane.normal);

				primitivesLength++;
			}
			else if (strcmp(JSONValueTempRef->data.dataString, "light") == 0) {
				// We found a point light
				sceneRef->lights[lightsLength] = malloc(sizeof(Light));
				sceneRef->lights[lightsLength]->type = POINTLIGHT_T;

				// Read the color
				if (JSONObject_get_value("color", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != ARRAY_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}

				if (JSONArray_to_V3(JSONValueTempRef->data.dataArray, &sceneRef->lights[lightsLength]->data.pointLight.color) != 0) {
					return 1;
				}

				// Check colors
				for (int j = 0; j < 3; j++) {
					if (sceneRef->lights[lightsLength]->data.pointLight.color.array[j] < 0) {
						fprintf(stderr, "Error: Color cannot be negative\n");
						return 1;
					}
				}

				// Read the position
				if (JSONObject_get_value("position", JSONObjectTempRef, &JSONValueTempRef) != 0) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}
				if (JSONValueTempRef->type != ARRAY_T) {
					fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
					return 1;
				}

				if (JSONArray_to_V3(JSONValueTempRef->data.dataArray, &sceneRef->lights[lightsLength]->data.pointLight.position) != 0) {
					return 1;
				}

				// Read the radialA2
				if (JSONObject_get_value("radial-a2", JSONObjectTempRef, &JSONValueTempRef) == 0) {
					if (JSONValueTempRef->type != NUMBER_T) {
						fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
						return 1;
					}

					sceneRef->lights[lightsLength]->data.pointLight.radialA2 = JSONValueTempRef->data.dataNumber;
				}
				else {
					sceneRef->lights[lightsLength]->data.pointLight.radialA2 = 1;
				}

				// Read the radialA1
				if (JSONObject_get_value("radial-a1", JSONObjectTempRef, &JSONValueTempRef) == 0) {
					if (JSONValueTempRef->type != NUMBER_T) {
						fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
						return 1;
					}

					sceneRef->lights[lightsLength]->data.pointLight.radialA1 = JSONValueTempRef->data.dataNumber;
				}
				else {
					sceneRef->lights[lightsLength]->data.pointLight.radialA1 = 0;
				}

				// Read the radialA0
				if (JSONObject_get_value("radial-a0", JSONObjectTempRef, &JSONValueTempRef) == 0) {
					if (JSONValueTempRef->type != NUMBER_T) {
						fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
						return 1;
					}

					sceneRef->lights[lightsLength]->data.pointLight.radialA0 = JSONValueTempRef->data.dataNumber;
				}
				else {
					sceneRef->lights[lightsLength]->data.pointLight.radialA0 = 0;
				}

				// Ensure that A0, A1, and A2 are not all 0
				if (sceneRef->lights[lightsLength]->data.pointLight.radialA0 == 0 &&
						sceneRef->lights[lightsLength]->data.pointLight.radialA1 == 0 &&
						sceneRef->lights[lightsLength]->data.pointLight.radialA2 == 0) {
					fprintf(stderr, "Error: Input scene light constants must have one constant not equal to 0\n");
					return 1;
				}

				if (sceneRef->lights[lightsLength]->data.pointLight.radialA0 < 0 ||
					sceneRef->lights[lightsLength]->data.pointLight.radialA1 < 0 ||
					sceneRef->lights[lightsLength]->data.pointLight.radialA2 < 0) {
					fprintf(stderr, "Error: Input scene light constants must not be negative\n");
					return 1;
				}

				// Read the angularA0 if it exists
				if (JSONObject_get_value("theta", JSONObjectTempRef, &JSONValueTempRef) == 0) {
					if (JSONValueTempRef->type != NUMBER_T) {
						fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
						return 1;
					}

					if (JSONValueTempRef->data.dataNumber != 0) {
						sceneRef->lights[lightsLength]->type = SPOTLIGHT_T;

						// Translate Theta into radians
						sceneRef->lights[lightsLength]->data.spotLight.theta = (float) (JSONValueTempRef->data.dataNumber * (M_PI/180));


						if (JSONObject_get_value("angular-a0", JSONObjectTempRef, &JSONValueTempRef) != 0) {
							fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
							return 1;
						}
						if (JSONValueTempRef->type != NUMBER_T) {
							fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
							return 1;
						}

						sceneRef->lights[lightsLength]->data.spotLight.angularA0 = JSONValueTempRef->data.dataNumber;

						if (sceneRef->lights[lightsLength]->data.spotLight.angularA0 < 0) {
							fprintf(stderr, "Error: Input scene light constants must not be negative\n");
							return 1;
						}

						// Read the direction
						if (JSONObject_get_value("direction", JSONObjectTempRef, &JSONValueTempRef) != 0) {
							fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
							return 1;
						}
						if (JSONValueTempRef->type != ARRAY_T) {
							fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
							return 1;
						}

						if (JSONArray_to_V3(JSONValueTempRef->data.dataArray, &sceneRef->lights[lightsLength]->data.spotLight.direction) != 0) {
							return 1;
						}

						// Normalize the direction
						v3_normalize(&sceneRef->lights[lightsLength]->data.spotLight.direction, &sceneRef->lights[lightsLength]->data.spotLight.direction);
					}
				}

				lightsLength++;
			}
			else {
				fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
				return 1;
			}
		}
		else {
			fprintf(stderr, "Error: Input scene JSON file contains invalid entries\n");
			return 1;
		}

		sceneRef->primitivesLength = primitivesLength;
		sceneRef->lightsLength = lightsLength;
	}

	return 0;
}