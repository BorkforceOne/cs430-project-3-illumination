//
// Created by Brandon Garling on 9/21/2016.
//

#include <stdlib.h>
#include <stdio.h>
#include "3dmath.h"
#include "raycaster.h"
#include "imaging.h"

/**
 * Allocates space in the imageRef specified for an image of the selected imageWidth and imageHeight.
 * Then raycasts a specified scene into the specified image.
 * @param sceneRef - The input scene to render
 * @param imageRef - The output image to write to
 * @param imageWidth - The height of the output image
 * @param imageHeight - The width of the output image
 * @return 0 if success, otherwise a failure occurred
 */
int raycast(Scene *sceneRef, Image* imageRef, int imageWidth, int imageHeight) {

	imageRef->width = (uint32_t) imageWidth;
	imageRef->height= (uint32_t) imageHeight;
	imageRef->pixmapRef = malloc(sizeof(RGBApixel) * imageWidth * imageHeight);

	double cameraHeight = sceneRef->camera.height;
	double cameraWidth = sceneRef->camera.width;

	V3 viewPlanePos = {0, 0, 1};
    V3 cameraPos = {0, 0, 0};

	double pixelHeight = cameraHeight/imageWidth;
	double pixelWidth = cameraWidth/imageHeight;

	V3 rayDirection = {0, 0, 0}; // The direction of our ray
    V3 point = {0, 0, 0}; // The point on the viewPlane that we intersect

	RGBAColor colorFound;

    point.data.Z = viewPlanePos.data.Z;
	for (int i=0; i<imageWidth; i++) {
        point.data.Y = -(viewPlanePos.data.Y - cameraHeight/2.0 + pixelHeight * (i + 0.5));
		for (int j=0; j<imageHeight; j++) {
            point.data.X = viewPlanePos.data.X - cameraWidth/2.0 + pixelWidth * (j + 0.5);
			v3_normalize(&point, &rayDirection); // normalization, find the ray direction
			shoot(&cameraPos, &rayDirection, sceneRef, &colorFound);
			shade(&colorFound, &imageRef->pixmapRef[i*imageHeight + j]);
		}
	}


	return 0;
}

/**
 * Shades a specific pixel based on the primitive provides
 * @param primitiveHitRef - The primitive that was hit during a call to the shoot function
 * @param pixel  - The pixel to color
 * @return 0 if success, otherwise a failure occurred
 */
int shade(RGBAColor* colorRef, RGBApixel *pixel) {
	pixel->r = colorRef->data.R;
	pixel->g = colorRef->data.G;
	pixel->b = colorRef->data.B;
	pixel->a = 255;
	return 0;
}

/**
 * Does the actual raytracing and sets the primitiveHit to a pointer to the primitive that was hit,
 * if any, when shooting the ray.
 * @param rayOriginRef - The origin of the ray
 * @param rayDirectionRef - The direction of the ray
 * @param sceneRef - A reference to the current scene
 * @param primitiveHit - A reference to a refrence of a primitive to set to the hit reference if a hit occurs
 * @return
 */
int shoot(V3 *rayOriginRef, V3 *rayDirectionRef, Scene *sceneRef, RGBAColor *foundColor) {
	Primitive *primitiveRef;
	Primitive *primitiveHitRef = NULL;
	set_color(foundColor, 0, 0, 0, 1);
	// Our current closest t value
	double primitive_t = INFINITY;
	// A possible t value replacement
	double t_possible;

	for (int i = 0; i < sceneRef->primitivesLength; i++) {
		t_possible = INFINITY;
		primitiveRef = sceneRef->primitives[i];

		if (primitiveRef->type == PLANE_T)
			t_possible = intersect_plane(&primitiveRef->data.plane, rayOriginRef, rayDirectionRef);
		else if (primitiveRef->type == SPHERE_T)
			t_possible = intersect_sphere(&primitiveRef->data.sphere, rayOriginRef, rayDirectionRef);
		if (t_possible > 0 && t_possible < primitive_t) {
			primitive_t = t_possible;
			primitiveHitRef = primitiveRef;
		}
	}

	if (primitiveHitRef != NULL) {
		// Calculate our new rayOrigin
		V3 color = {0, 0, 0};
		// Set the ambient color
		//set_color(&color, 50, 50, 50, 255);

		V3 newRayOrigin;
		V3 newRayDirection;
		v3_scale(rayDirectionRef, primitive_t, &newRayOrigin);
		v3_add(rayOriginRef, &newRayOrigin, &newRayOrigin);
		// Shadow test
		for (int i = 0; i < sceneRef->lightsLength; i++) {
			v3_subtract(&sceneRef->lights[i]->data.pointLight.position, &newRayOrigin, &newRayDirection);
			v3_normalize(&newRayDirection, &newRayDirection);
			double light_t = INFINITY;
			for (int j = 0; j < sceneRef->primitivesLength; j++) {
				t_possible = INFINITY;
				primitiveRef = sceneRef->primitives[j];
				// Skip the current object
				if (primitiveRef == primitiveHitRef)
					continue;

				if (primitiveRef->type == PLANE_T)
					t_possible = intersect_plane(&primitiveRef->data.plane, &newRayOrigin, &newRayDirection);
				else if (primitiveRef->type == SPHERE_T)
					t_possible = intersect_sphere(&primitiveRef->data.sphere, &newRayOrigin, &newRayDirection);
				if (t_possible > 0 && t_possible < light_t)
					light_t = t_possible;
			}

			if (light_t != INFINITY)
				// Our light is in shadow
				continue;

			if (primitive_t > light_t)
				continue;

			// N, L, R, V

			// Normal
			V3 N;
			// Light_position - newRayOrigin;
			V3 L;
			v3_subtract(&sceneRef->lights[i]->data.pointLight.position, &newRayOrigin, &L);
			v3_normalize(&L, &L);
			// Reflection of L
			V3 R;
			// RayDirection
			V3 V;
			v3_distance(&sceneRef->lights[i]->data.pointLight.position, &newRayOrigin, &light_t);
			v3_copy(rayDirectionRef, &V);
			v3_normalize(&V, &V);

			switch(primitiveHitRef->type) {
				case PLANE_T:
					v3_copy(&primitiveHitRef->data.plane.normal, &N);
					break;

				case SPHERE_T:
					v3_subtract(&newRayOrigin, &primitiveHitRef->data.sphere.position, &N);
					v3_normalize(&N, &N);
					break;
			}
			v3_reflect(&L, &N, &R);
			V3 lightContribution;
			V3 diffuse;
			V3 specular;
			double frad;
			double fang;
			calculate_diffuse(&N, &L, sceneRef->lights[i], &diffuse);
			// multiply diffuse by each part of the input
			diffuse.array[0] *= primitiveHitRef->data.plane.diffuseColor.array[0];
			diffuse.array[1] *= primitiveHitRef->data.plane.diffuseColor.array[1];
			diffuse.array[2] *= primitiveHitRef->data.plane.diffuseColor.array[2];
			//calculate_specular(&specular);
			calculate_frad(sceneRef->lights[i], light_t, &frad);
			calculate_fang(&fang);
			//v3_add(&diffuse, &specular, &lightContribution);
			v3_copy(&diffuse, &lightContribution);
			v3_scale(&lightContribution, frad * fang, &lightContribution);
			color.array[0] += lightContribution.array[0];
			color.array[1] += lightContribution.array[1];
			color.array[2] += lightContribution.array[2];
		}

		// Figure out lighting
		foundColor->data.R = (uint8_t) (clamp(color.array[0])*255);
		foundColor->data.G = (uint8_t) (clamp(color.array[1])*255);
		foundColor->data.B = (uint8_t) (clamp(color.array[2])*255);
		foundColor->data.A = 1;

		//foundColor->data.R = (uint8_t) (clamp(primitiveHitRef->data.plane.diffuseColor.array[0])*255);
		//foundColor->data.G = (uint8_t) (clamp(primitiveHitRef->data.plane.diffuseColor.array[1])*255);
		//foundColor->data.B = (uint8_t) (clamp(primitiveHitRef->data.plane.diffuseColor.array[2])*255);
		//foundColor->data.A = 1;

	}

	return 0;
}

double clamp(double a) {
	if (a < 0)
		return 0;
	if (a > 1)
		return 1;
	return a;
}

void calculate_frad(Light *light, double distance, double *result) {
	*result = 1/(light->data.pointLight.radialA2*pow(distance, 2) +
			  light->data.pointLight.radialA1*distance +
			  light->data.pointLight.radialA0);
}

void calculate_fang(double *result) {
	*result = 1;
}

void calculate_diffuse(V3 *N, V3 *L, Light* light, V3* result) {
	double s;
	v3_dot(N, L, &s);
	if (s > 0)
		v3_scale(&light->data.pointLight.color, s, result);
	else {
		result->array[0] = 0;
		result->array[1] = 0;
		result->array[2] = 0;
	}
}

void set_color(RGBAColor* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	color->data.R = r;
	color->data.G = g;
	color->data.B = b;
	color->data.A = a;
}

void copy_color(RGBAColor* srcColor, RGBAColor* dstColor) {
	dstColor->data.R = srcColor->data.R;
	dstColor->data.G = srcColor->data.G;
	dstColor->data.B = srcColor->data.B;
	dstColor->data.A = srcColor->data.A;
}

double intersect_point_light(PointLight *pointLightRef, V3 *rayOriginRef) {
	double t;
	v3_distance(&pointLightRef->position, rayOriginRef, &t);
	return t;
}

double intersect_sphere(Sphere *sphereRef, V3 *rayOriginRef, V3 *rayDirectionRef) {
	double B = 2 * (rayDirectionRef->data.X * (rayOriginRef->data.X - sphereRef->position.data.X) + rayDirectionRef->data.Y*(rayOriginRef->data.Y - sphereRef->position.data.Y) + rayDirectionRef->data.Z*(rayOriginRef->data.Z - sphereRef->position.data.Z));
	double C = pow(rayOriginRef->data.X - sphereRef->position.data.X, 2) + pow(rayOriginRef->data.Y - sphereRef->position.data.Y, 2) + pow(rayOriginRef->data.Z - sphereRef->position.data.Z, 2) - pow(sphereRef->radius, 2);

	double discriminant = (pow(B, 2) - 4*C);
	if (discriminant < 0) {
		// No intersection
		return INFINITY;
	}

	double t_possible = (-B + sqrt(discriminant))/2;
	double t_possible2 = (-B - sqrt(discriminant))/2;
	if (t_possible || t_possible2 > 0) {
		if (t_possible > t_possible2)
			return t_possible2;
		else
			return t_possible;
	}

	return INFINITY;
}

double intersect_plane(Plane *planeRef, V3 *rayOriginRef, V3 *rayDirectionRef) {
	double Vd;
	double V0;
	V3 vectorTemp;
	v3_subtract(rayOriginRef, &planeRef->position, &vectorTemp);
	v3_dot(&planeRef->normal, &vectorTemp, &Vd);

	if (Vd == 0) {
		// No intersection!
		return INFINITY;
	}

	v3_dot(&planeRef->normal, rayDirectionRef, &V0);

	double t_possible = -(Vd / V0);
	if (t_possible > 0)
		return t_possible;

	return INFINITY;
}