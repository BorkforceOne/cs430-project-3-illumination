//
// Created by Brandon Garling on 9/21/2016.
//

#ifndef CS430_PROJECT_2_BASIC_RAYCASTER_3DMATH_H
#define CS430_PROJECT_2_BASIC_RAYCASTER_3DMATH_H

#include <math.h>

/**
 * A three dimensional vector struct
 */
typedef union V3 {
	struct {
		double X, Y, Z;
	} data;
	double array[3];
} V3;

/**
 * Perform a vector add operation a + b = addResult
 * @param a - The first vector
 * @param b - The second vector
 * @param result - The result of the addition is stored in this vector
 */
static inline void v3_add(V3 *a,V3 *b, V3 *result) {
	result->data.X = a->data.X + b->data.X;
	result->data.Y = a->data.Y + b->data.Y;
	result->data.Z = a->data.Z + b->data.Z;
}

/**
 * Perform a vector subtract operation a - b = subtractResult
 * @param a - The first vector
 * @param b - The second vector
 * @param subtractResult - The result of the subtraction is stored in this vector
 */
static inline void v3_subtract(V3 *a,V3 *b, V3 *result) {
	result->data.X = a->data.X - b->data.X;
	result->data.Y = a->data.Y - b->data.Y;
	result->data.Z = a->data.Z - b->data.Z;
}

/**
 * Perform a vector scaling a*s = scaleResult
 * @param a - The first vector
 * @param b - The second vector
 * @param scaleResult - The result of the subtraction is stored in this vector
 */
static inline void v3_scale(V3 *a, double s, V3 *result) {
	result->data.X = a->data.X * s;
	result->data.Y = a->data.Y * s;
	result->data.Z = a->data.Z * s;
}

/**
 * Perform a vector dot operation a (dot) b = result
 * @param a - The first vector
 * @param b - The second vector
 * @param result - The result of the dot operation is stored in this vector
 */
static inline void v3_dot(V3 *a, V3 *b, double *result) {
	*result = a->data.X * b->data.X + a->data.Y * b->data.Y + a->data.Z * b->data.Z;
}

/**
 * Perform a vector cross operation a x b = result
 * @param a - The first vector
 * @param b - The second vector
 * @param result - The result of the cross operation is stored in this vector
 */
static inline void v3_cross(V3 *a, V3 *b, V3 *result) {
	result->data.X = a->data.Y * b->data.Z - a->data.Z * b->data.Y;
	result->data.Y = a->data.Z * b->data.X - a->data.X * b->data.Z;
	result->data.Z = a->data.X * b->data.Y - a->data.Y * b->data.X;
}

/**
 * Calculate the magnitude of the input vector
 * @param a - The vector to calculate the input vector of
 * @param result - The result of the magnitude calculation
 */
static inline void v3_magnitude(V3 *a, double *result) {
	*result = sqrt(a->data.X*a->data.X + a->data.Y*a->data.Y + a->data.Z*a->data.Z);
}

/**
 * Performs a normalization operation on the input vector
 * @param a - The vector to normalize
 * @param b - The result of the normalization operation calculation
 */
static inline void v3_normalize(V3 *a, V3 *b) {
	double scale;
	v3_magnitude(a, &scale);
	v3_scale(a, 1/scale, b);
}

/**
 * Copy a source vector to a destination vector
 * @param src - The source vector to copy from
 * @param dst - The destination vector to copy to
 */
static inline void v3_copy(V3 *src, V3 *dst) {
	dst->data.X = src->data.X;
	dst->data.Y = src->data.Y;
	dst->data.Z = src->data.Z;
}

static inline void v3_reflect(V3 *a, V3* n, V3* result) {
	double s;
	v3_dot(a, n, &s);
	v3_scale(n, -2 * s, result);
	v3_add(result, a, result);
}

static inline void v3_distance(V3 *a, V3 *b, double *result) {
	*result = sqrt(pow(b->data.X - a->data.X, 2) +
			  pow(b->data.Y - a->data.Y, 2) +
			  pow(b->data.Z - a->data.Z, 2));
}

#endif //CS430_PROJECT_2_BASIC_RAYCASTER_3DMATH_H
