//
// Created by Brandon Garling on 9/21/2016.
//

#ifndef CS430_PROJECT_2_BASIC_RAYCASTER_RAYTRACER_H
#define CS430_PROJECT_2_BASIC_RAYCASTER_RAYTRACER_H

#include "3dmath.h"

/**
 * Supported Primitive Types
 */
typedef enum PrimitiveType_t {
	CAMERA_T,
	SPHERE_T,
	PLANE_T
} PrimitiveType_t;


/**
 * Camera Struct
 */
typedef struct Camera {
	double width;
	double height;
} Camera;

/**
 * Sphere Struct
 */
typedef struct Sphere {
	double radius;
	V3 color;
	V3 position;
} Sphere;

/**
 * Plane Struct
 */
typedef struct Plane {
	V3 color;
	V3 position;
	V3 normal;
} Plane;

/**
 * Primitive Struct
 */
typedef struct Primitive {
	PrimitiveType_t type;
	union {
		Camera camera;
		Plane plane;
		Sphere sphere;
	} data;
} Primitive;

/**
 * Scene Struct
 */
typedef struct Scene {
	Camera camera;
	Primitive** primitives;
	int primitivesLength;
} Scene;

// Define needed structure prototypes
typedef struct RGBApixel RGBApixel;
typedef struct JSONArray JSONArray;
typedef struct Image Image;

int raycast(Scene *sceneRef, Image* imageRef, int imageWidth, int imageHeight);
int shade(Primitive* primitiveHitRef, RGBApixel *pixel);
int shoot(V3 *rayOriginRef, V3 *rayDirectionRef, Scene *sceneRef, Primitive **primitiveHit);

#endif //CS430_PROJECT_2_BASIC_RAYCASTER_RAYTRACER_H
