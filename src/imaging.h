//
// Created by Brandon Garling on 9/20/2016.
//

#ifndef CS430_PROJECT_2_BASIC_RAYCASTER_IMAGING_H
#define CS430_PROJECT_2_BASIC_RAYCASTER_IMAGING_H

#include <stdint.h>

/**
 * RGBA Pixel - Used to store a single pixel value
 */
typedef struct RGBApixel {
	uint8_t r, g, b, a;
} RGBApixel;

/**
 * RGBAColor - Can be used to address RGBA colors
 */
typedef struct RGBAColor {
	union {
		struct {
			uint8_t R, G, B, A;
		} data;
		uint8_t array[4];
	};
} RGBAColor;

/**
 * Image - An image containing a width, height, and pixmap
 */
typedef struct Image {
	uint32_t width, height;
	RGBApixel *pixmapRef;
} Image;

#endif //CS430_PROJECT_2_BASIC_RAYCASTER_IMAGING_H
