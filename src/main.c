#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "json.h"
#include "raycaster.h"
#include "ppm.h"
#include "raycaster_helpers.h"
#include "constants.h"

/**
 * Determine if the input string is a number, this does not currently support
 * floating point numbers.
 * @param string - The string to check
 * @return 1 if it is a number, 0 if it is not a number
 */
int isinteger(char *string) {
	int i = 0;
	do {
		if (!isdigit(string[i]) && !(string[i] == '-' && i == 0)) {
			return FALSE;
		}
		i++;
	} while (string[i] != '\0');
	return TRUE;
}

/**
 * Show a simple help message about the usage of this program
 */
void show_help() {
	printf("Usage: raycast <render_width> <render_height> <input_scene> <output_file>\n");
	printf("\t render_width: The width of the image to render\n");
	printf("\t render_height: The height of the image to render\n");
	printf("\t input_scene: The input scene file in a supported JSON format\n");
	printf("\t output_file: The location to write the output PPM P6 image\n");
	printf("\n");
	printf("\t Example: raycast 1920 1080 scene.json out.ppm\n");
}

/**
 * The main enchilada, do all the things!
 */
int main (int argc, char *argv[]) {
	if (argc != 5) {
        fprintf(stderr, "Error: Not enough arguments provided\n");
		show_help();
		return 1;
	}

	int imageWidth = atoi(argv[1]);
	int imageHeight = atoi(argv[2]);
	char *inputFname = argv[3];
	char *outputFname = argv[4];

	if (!isinteger(argv[1]) || imageHeight <= 0) {
        fprintf(stderr, "Error: Argument render_height must be an positive integer\n");
        show_help();
		return 1;
	}

	if (!isinteger(argv[2]) || imageWidth <= 0) {
        fprintf(stderr, "Error: Argument render_width must be an positive integer\n");
		show_help();
		return 1;
	}

	// Read the input JSON file
	JSONValue JSONRoot;
	printf("[INFO] Reading input scene file '%s'\n", inputFname);
	if (read_json(inputFname, &JSONRoot) != 0)
		return 1;

	// Convert the JSON file to a scene
	Scene scene;
	printf("[INFO] Creating scene from input scene file\n");
	if (create_scene_from_JSON(&JSONRoot, &scene) != 0)
		return 1;

	// Raycast the scene into an image
	Image image;
	printf("[INFO] Raycasting scene into image\n");
	if (raycast(&scene, &image, imageWidth, imageHeight) != 0)
		return 1;

	// Write the image out to the specified file
	printf("[INFO] Saving image (PPM P6) to output file '%s'\n", outputFname);
	if (save_ppm_p6_image(&image, outputFname) != 0)
		return 1;

	printf("[INFO] Finished!\n");
	return 0;
}
