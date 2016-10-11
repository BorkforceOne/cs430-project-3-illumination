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
	int i = 0;
	// Our current closest t value
	double t = INFINITY;
	// A possible t value replacement
	double t_possible;

	for (i = 0; i < sceneRef->primitivesLength; i++) {
		t_possible = INFINITY;
		primitiveRef = sceneRef->primitives[i];

		if (primitiveRef->type == PLANE_T)
			t_possible = intersect_plane(&primitiveRef->data.plane, rayOriginRef, rayDirectionRef);
		else if (primitiveRef->type == SPHERE_T)
			t_possible = intersect_sphere(&primitiveRef->data.sphere, rayOriginRef, rayDirectionRef);
		if (t_possible > 0 && t_possible < t) {
			t = t_possible;
			primitiveHitRef = primitiveRef;
		}
	}

	if (primitiveHitRef != NULL) {
		// Calculate our new rayOrigin
		V3 newRayOrigin;
		v3_scale(rayDirectionRef, t, &newRayOrigin);
		v3_add(rayOriginRef, &newRayOrigin, &newRayOrigin);
		// Figure out lighting
		Light *lightRef;
		int totalLights = 0;
		foundColor->data.R = 0;
		foundColor->data.G = 0;
		foundColor->data.B = 0;
		foundColor->data.A = 1;
		V3 lightInfluence;
		V3 lightDiffuse;
		for (i = 0; i < sceneRef->lightsLength; i++) {
			lightRef = sceneRef->lights[i];
			if (lightRef->type == POINTLIGHT_T) {
				t_possible = intersect_point_light(&lightRef->data.pointLight, &newRayOrigin);
				v3_copy(&lightRef->data.pointLight.color, &lightInfluence);
				v3_scale(&lightInfluence, 1/(lightRef->data.pointLight.radialA2*pow(t_possible, 2) + lightRef->data.pointLight.radialA1)*t_possible + lightRef->data.pointLight.radialA0, &lightInfluence);
				v3_add(&lightDiffuse, &lightInfluence, &lightDiffuse);
				totalLights++;
			}
		}
		v3_add(&primitiveHitRef->data.plane.diffuseColor, &lightDiffuse, &lightDiffuse);
		v3_scale(&lightDiffuse, 1/totalLights, &lightDiffuse);
		foundColor->data.R = lightDiffuse.data.X*255;
		foundColor->data.G = lightDiffuse.data.Y*255;
		foundColor->data.B = lightDiffuse.data.Z*255;
	}

	return 0;
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