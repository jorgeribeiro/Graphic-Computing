#ifndef _rt_H
#define _rt_H

#include <vector>
#include <map>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TriangleMesh.h"    // model loading
#include "Shader.h"          // compiling shaders
#include "camera.h"			 // camera setup
#include "utils.h"           // generic helper functions
#include "scene_constants.h" // material and light properties
#include "path_to_files.h"   // paths to textures and shaders


///////////////////////////////////////////////////////////////////////////////
//                           Load bearing functions                          //
///////////////////////////////////////////////////////////////////////////////

/**
 * The main function of the application - handles drawing
 * - clears the screen
 * - binds the shader
 * - activates textures
 * - sends uniform variables, vertex attributes to the shader
 * - draws the scene
 */
void display_handler(void);

/**
 * Callback for keyboard events
 * Changes the perspective by translating and rotating the view matrix in
 * response to user input
 *
 * Supported keys:
 * - ``q w e r t y`` to translate the model in the +/- direction of the 3 axes
 * - ``a s d f g h`` to rotate the model in the +/- direction of the 3 axes
 * - ``(space)`` to reset to the default perspective
 */
void keyboard_handler(unsigned char key, int x, int y);

/**
 * The function that is called after the application closes
 */
void cleanup(void);


///////////////////////////////////////////////////////////////////////////////
//                              Helper functions                             //
///////////////////////////////////////////////////////////////////////////////

/**
 * Read a ``bmp`` file from |texture_path|
 * Load the texture into video memory and bind it to |textureID|
 */
void setup_texture(char *texture_path, GLuint *textureID);

/**
 * Create a buffer object for vertex positions
 * Bind it to the |vertex_position_buffer| global variable
 */
void setup_vertex_position_buffer_object(void);

/**
 * Create a buffer object for vertex uv coordinates (for texture mapping)
 * Bind it to the |vertex_uv_buffer| global variable
 */
void setup_vertex_uv_buffer_object(void);

/**
 * Compute normals for all the vertices in the application's triangle mesh
 *
 * If |smoothed| is set, the vertex normals are the average of the face normals
 * of the triangles that vertex participates in. If |smoothed| is not set, the
 * averaging step is left out.
 *
 * Create a buffer object for vertex normals
 * Bind it to the |vertex_normal_buffer| global variable
 */
void setup_vertex_normal_buffer_object(bool smoothed);

/**
 * Returns the projection matrix as it was at the start of the application
 * The projection matrix is used to convert from view to screen coordinates
 */
glm::mat4 get_default_projectionMatrix(void);

/**
 * Returns the view matrix as it was at the start of the application
 * The view matrix is used to convert from world to view coordinates
 */
glm::mat4 get_default_viewMatrix(void);

/**
 * Returns the model matrix as it was at the start of the application
 * The model matrix is used to convert from object to world coordinates
 */
glm::mat4 get_default_modelMatrix(void);

/**
 * Returns the normal matrix as it was at the start of the application
 * The normal matrix is the transpose of the inverse of the model-view matrix
 */
glm::mat3 get_default_normalMatrix(void);

#endif
