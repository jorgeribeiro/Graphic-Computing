// Very simple shader that colors points relative to their depths
#version 120

varying float depth;

void main(void) {
    // the farther back, the darker the color
    float d = 1.0 - depth;

    // set pixel color in OpenGL
    gl_FragColor = vec4(d, d, d, 1.0);
}
