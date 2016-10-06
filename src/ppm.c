//
// Created by Brandon Garling on 9/20/2016.
//

#include <stdlib.h>
#include <stdio.h>
#include "imaging.h"

/**
 * Write the specified image to a file using PPM P6 format
 * @param imageRef - The image to write
 * @param fname - The output filename
 * @return 0 if success, otherwise a failure occurred
 */
int save_ppm_p6_image(Image *imageRef, char *fname) {
	FILE* fp = fopen(fname, "w");
	int i;
	int j;
	// a small buffer to hold our values for the pixel to be written
	char buffer[3];
	if (fp) {
		// write the magic number
		fprintf(fp, "P6\n");
		// write the width and height
		fprintf(fp, "%i %i\n", imageRef->width, imageRef->height);
		// write the max color
		fprintf(fp, "255\n");
		for (i=0; i<imageRef->height; i++) {
			for (j=0; j<imageRef->width; j++) {
				// grab the correct pixel
				RGBApixel pixel = imageRef->pixmapRef[i*imageRef->width + j];
				// copy the rgb values to our buffer
				buffer[0] = pixel.r;
				buffer[1] = pixel.g;
				buffer[2] = pixel.b;
				// write the pixel to the file
				fwrite(buffer, sizeof(uint8_t), 3, fp);
			}
		}
		// close the file
		fclose(fp);
		return 0;
	}
	else {
		fprintf(stderr, "Error: File '%s' could not be opened for writing\n", fname);
		return 1;
	}
}