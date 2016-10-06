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

	Primitive *primitiveHit;

    point.Z = viewPlanePos.Z;
	for (int i=0; i<imageWidth; i++) {
        point.Y = -(viewPlanePos.Y - cameraHeight/2.0 + pixelHeight * (i + 0.5));
		for (int j=0; j<imageHeight; j++) {
            point.X = viewPlanePos.X - cameraWidth/2.0 + pixelWidth * (j + 0.5);
			v3_normalize(&point, &rayDirection); // normalization, find the ray direction
			shoot(&cameraPos, &rayDirection, sceneRef, &primitiveHit);
			shade(primitiveHit, &imageRef->pixmapRef[i*imageHeight + j]);
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
int shade(Primitive* primitiveHitRef, RGBApixel *pixel) {
	if (primitiveHitRef != NULL) {
		if (primitiveHitRef->type == SPHERE_T) {
			pixel->r = (uint8_t)(primitiveHitRef->data.sphere.color.X*255);
			pixel->g = (uint8_t)(primitiveHitRef->data.sphere.color.Y*255);
			pixel->b = (uint8_t)(primitiveHitRef->data.sphere.color.Z*255);
			pixel->a = 255;
			return 0;
		}
		if (primitiveHitRef->type == PLANE_T) {
			pixel->r = (uint8_t)(primitiveHitRef->data.plane.color.X*255);
			pixel->g = (uint8_t)(primitiveHitRef->data.plane.color.Y*255);
			pixel->b = (uint8_t)(primitiveHitRef->data.plane.color.Z*255);
			pixel->a = 255;
			return 0;
		}
	}
	pixel->r = 0;
	pixel->g = 0;
	pixel->b = 0;
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
int shoot(V3 *rayOriginRef, V3 *rayDirectionRef, Scene *sceneRef, Primitive **primitiveHit) {
	Primitive *primitiveRef;
	Plane *planeRef;
	Sphere *sphereRef;
	// Reset the hit primitive
	*primitiveHit = NULL;
	int i = 0;
	// Our current closest t value
	double t = INFINITY;
	// A possible t value replacement
	double t_possible;

	for (i = 0; i < sceneRef->primitivesLength; i++) {
		primitiveRef = sceneRef->primitives[i];

		if (primitiveRef->type == PLANE_T) {
			planeRef = &primitiveRef->data.plane;
			double Vd;
			double V0;
			V3 vectorTemp;
			v3_subtract(rayOriginRef, &planeRef->position, &vectorTemp);
			v3_dot(&planeRef->normal, &vectorTemp, &Vd);

			if (Vd == 0) {
				// No intersection!
				continue;
			}

			v3_dot(&planeRef->normal, rayDirectionRef, &V0);

			t_possible = -(Vd / V0);
			if (t_possible < t && t_possible > 0) {
				t = t_possible;
				*primitiveHit = primitiveRef;
			}
		}
		else if (primitiveRef->type == SPHERE_T) {
			sphereRef = &primitiveRef->data.sphere;
			double B = 2 * (rayDirectionRef->X * (rayOriginRef->X - sphereRef->position.X) + rayDirectionRef->Y*(rayOriginRef->Y - sphereRef->position.Y) + rayDirectionRef->Z*(rayOriginRef->Z - sphereRef->position.Z));
			double C = pow(rayOriginRef->X - sphereRef->position.X, 2) + pow(rayOriginRef->Y - sphereRef->position.Y, 2) + pow(rayOriginRef->Z - sphereRef->position.Z, 2) - pow(sphereRef->radius, 2);

			double discriminant = (pow(B, 2) - 4*C);
			if (discriminant < 0) {
				// No intersection
				continue;
			}

			t_possible = (-B + sqrt(discriminant))/2;

			if (t_possible < t && t_possible > 0) {
				t = t_possible;
				*primitiveHit = primitiveRef;
			}

			t_possible = (-B - sqrt(discriminant))/2;

			if (t_possible < t && t_possible > 0) {
				t = t_possible;
				*primitiveHit = primitiveRef;
			}
		}
	}

	return 0;
}