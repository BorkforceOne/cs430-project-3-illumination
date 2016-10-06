//
// Created by Brandon Garling on 9/21/2016.
//

#ifndef CS430_PROJECT_2_BASIC_RAYCASTER_3DMATH_H
#define CS430_PROJECT_2_BASIC_RAYCASTER_3DMATH_H

#include <math.h>

/**
 * A three dimensional vector struct
 */
typedef struct V3 {
	double X;
	double Y;
	double Z;
} V3;

/**
 * Perform a vector add operation a + b = addResult
 * @param a - The first vector
 * @param b - The second vector
 * @param result - The result of the addition is stored in this vector
 */
static inline void v3_add(V3 *a,V3 *b, V3 *result) {
	result->X = a->X + b->X;
	result->Y = a->Y + b->Y;
	result->Z = a->Z + b->Z;
}

/**
 * Perform a vector subtract operation a - b = subtractResult
 * @param a - The first vector
 * @param b - The second vector
 * @param subtractResult - The result of the subtraction is stored in this vector
 */
static inline void v3_subtract(V3 *a,V3 *b, V3 *result) {
	result->X = a->X - b->X;
	result->Y = a->Y - b->Y;
	result->Z = a->Z - b->Z;
}

/**
 * Perform a vector scaling a*s = scaleResult
 * @param a - The first vector
 * @param b - The second vector
 * @param scaleResult - The result of the subtraction is stored in this vector
 */
static inline void v3_scale(V3 *a, double s, V3 *result) {
	result->X = a->X * s;
	result->Y = a->Y * s;
	result->Z = a->Z * s;
}

/**
 * Perform a vector dot operation a (dot) b = result
 * @param a - The first vector
 * @param b - The second vector
 * @param result - The result of the dot operation is stored in this vector
 */
static inline void v3_dot(V3 *a, V3 *b, double *result) {
	*result = a->X * b->X + a->Y * b->Y + a->Z * b->Z;
}

/**
 * Perform a vector cross operation a x b = result
 * @param a - The first vector
 * @param b - The second vector
 * @param result - The result of the cross operation is stored in this vector
 */
static inline void v3_cross(V3 *a, V3 *b, V3 *result) {
	result->X = a->Y * b->Z - a->Z * b->Y;
	result->Y = a->Z * b->X - a->X * b->Z;
	result->Z = a->X * b->Y - a->Y * b->X;
}

/**
 * Calculate the magnitude of the input vector
 * @param a - The vector to calculate the input vector of
 * @param result - The result of the magnitude calculation
 */
static inline void v3_magnitude(V3 *a, double *result) {
	*result = sqrt(a->X*a->X + a->Y*a->Y + a->Z*a->Z);
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
	dst->X = src->X;
	dst->Y = src->Y;
	dst->Z = src->Z;
}

#endif //CS430_PROJECT_2_BASIC_RAYCASTER_3DMATH_H
