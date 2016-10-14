//
// Created by Brandon Garling on 9/21/2016.
//

#ifndef CS430_PROJECT_2_BASIC_RAYCASTER_RAYTRACER_H
#define CS430_PROJECT_2_BASIC_RAYCASTER_RAYTRACER_H

#include "3dmath.h"
#include "imaging.h"

/**
 * Supported Primitive Types
 */
typedef enum PrimitiveType_t {
	SPHERE_T,
	PLANE_T
} PrimitiveType_t;

/**
 * Supported Light Types
 */
typedef enum LightType_t {
	POINTLIGHT_T
} LightType_t;

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
	V3 diffuseColor;
	V3 specularColor;
	V3 position;
	double radius;
} Sphere;

/**
 * Plane Struct
 */
typedef struct Plane {
	V3 diffuseColor;
	V3 specularColor;
	V3 position;
	V3 normal;
} Plane;

/**
 * Primitive Struct
 */
typedef struct Primitive {
	PrimitiveType_t type;
	union {
		Plane plane;
		Sphere sphere;
	} data;
} Primitive;

/**
 * Point Light
 */

typedef struct PointLight {
	V3 color;
	V3 position;
	float radialA2;
	float radialA1;
	float radialA0;
} PointLight;

/**
 * Light Struct
 */
typedef struct Light {
	LightType_t type;
	union {
		PointLight pointLight;
	} data;
} Light;

/**
 * Scene Struct
 */
typedef struct Scene {
	Camera camera;
	Primitive** primitives;
	Light** lights;
	int primitivesLength;
	int lightsLength;
} Scene;

// Define needed structure prototypes
typedef struct JSONArray JSONArray;

int raycast(Scene *sceneRef, Image* imageRef, int imageWidth, int imageHeight);
int shade(RGBAColor* colorRef, RGBApixel *pixel);
int shoot(V3 *rayOriginRef, V3 *rayDirectionRef, Scene *sceneRef, RGBAColor *foundColor);
double intersect_sphere(Sphere *sphereRef, V3 *rayOriginRef, V3 *rayDirectionRef);
double intersect_plane(Plane *planeRef, V3 *rayOriginRef, V3 *rayDirectionRef);
double intersect_point_light(PointLight *pointLightRef, V3 *rayOriginRef);
double clamp(double a);
void calculate_frad(Light *light, double distance, double *result);
void calculate_fang(double *result);
void calculate_diffuse(V3 *N, V3 *L, Light* light, V3* result);
void set_color(RGBAColor* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

#endif //CS430_PROJECT_2_BASIC_RAYCASTER_RAYTRACER_H
