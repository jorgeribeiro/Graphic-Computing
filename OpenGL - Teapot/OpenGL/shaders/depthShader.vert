// Very simple shader that colors points relative to their depths
#version 120

uniform mat4 projectionMatrix, viewMatrix, modelMatrix;

attribute vec3 vertex_position;

varying float depth;

void main(void) {
    vec4 position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex_position, 1.0);

    // pass variables
    depth = position.z / position.w;

    // set vertex position in OpenGL
    gl_Position = position;
}
