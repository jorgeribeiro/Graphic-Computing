// Compute colors as normal and discretize to fixed pallette before rendering
#version 120

uniform mat4 projectionMatrix, viewMatrix, modelMatrix;
uniform mat3 normalMatrix;
uniform vec3 materialAmbient, materialDiffuse, materialSpecular;
uniform float materialShininess, constantAttenuation, linearAttenuation;

attribute vec3 vertex_position, vertex_normal;
attribute vec2 vertex_uv;

varying vec3 vertex_color, position, normal;
varying vec2 uv;

void main(void) {
    vec4 vertex = vec4(vertex_position, 1.0);

    // pass variables
    normal = normalize(normalMatrix * vertex_normal);
    position = vec3(viewMatrix * modelMatrix * vertex);
    uv = vertex_uv;
    vertex_color = materialAmbient + materialDiffuse;

    // set vertex position in OpenGL
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertex;
}
