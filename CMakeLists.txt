cmake_minimum_required(VERSION 3.6)
project(cs430_project_3_illumination)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.c src/ppm.c src/constants.h src/ppm.h src/imaging.h src/json.c src/json_parsers.c src/json_parsers.h src/json_helpers.c src/json_helpers.h src/helpers.h src/helpers.c src/ppm_helpers.h src/ppm_helpers.c src/json.h src/raycaster.h src/raycaster.c src/3dmath.h src/raycaster_helpers.c src/raycaster_helpers.h)
add_executable(cs430_project_3_illumination ${SOURCE_FILES})