# CS430 Project 3 - Illumination

This project implements a simple raycasting algorithm with lights (point lights, spot lights) and shadows that allows raycasting primitive objects (spheres, planes) defined in an input file in JSON format into a PPM image file.

### Building

```sh
$ git clone https://github.com/bjg96/cs430-project-3-illumination.git
$ cd cs430-project-3-illumination
$ make
```

### Usage

```sh
$ ./raycast <render_width> <render_height> <input_scene> <output_file>
$        render_width: The width of the image to render
$        render_height: The height of the image to render
$        input_scene: The input scene file in a supported JSON format
$        output_file: The location to write the output PPM P6 image
$
$        Example: raycast 1920 1080 scene.json out.ppm
```
