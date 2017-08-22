// Perturb normal vectors using a bump map
#version 120

uniform mat4 projectionMatrix, viewMatrix, modelMatrix;
uniform mat3 normalMatrix;
uniform vec3 materialAmbient, materialDiffuse;
uniform vec3 lightAmbient, lightDiffuse, lightPosition, lightGlobal;

attribute vec3 vertex_position, vertex_normal;
attribute vec2 vertex_uv;

varying vec3 ambientGlobal, ambient, diffuse, position, normal, tangent, binormal;
varying vec2 uv;

void main(void) {
    vec4 vertex = vec4(vertex_position, 1.0);

    // transform normal and position for fragment shader
    normal = normalize(normalMatrix * vertex_normal);
    position = vec3(viewMatrix * modelMatrix * vertex);

    // base colors don't change per pixel - can compute now
    ambient = materialAmbient * lightAmbient;
    diffuse = materialDiffuse * lightDiffuse;
    ambientGlobal = materialAmbient * lightGlobal;

    // approximate tangent and binormal
    vec3 c1 = cross(vertex_normal, vec3(0.0, 0.0, 1.0));
    vec3 c2 = cross(vertex_normal, vec3(0.0, 1.0, 0.0));
    vec3 T = normalize(length(c1) > length(c2) ? c1 : c2);
    vec3 B = normalize(cross(vertex_normal, T));

    // pass variables
    uv = vertex_uv;
    tangent = T;
    binormal = B;

    // set vertex position in OpenGL
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vertex;
}
