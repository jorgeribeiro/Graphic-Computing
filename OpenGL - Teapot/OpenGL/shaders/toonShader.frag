// Compute colors as normal and discretize to fixed pallette before rendering
#version 120

uniform sampler2D texture0;
uniform int useTexture;
uniform vec3 lightPosition;

varying vec3 position, vertex_color, normal;
varying vec2 uv;

void main(void) {
    // compute perceived intensity at pixel
    vec3 N = normalize(normal);
    vec3 L = normalize(lightPosition - position);
    float intensity = max(dot(N, L), 0.0);

    // discretize colors based on perceived intensity
    vec3 discrete_color;
    if (intensity > 0.98) {
        discrete_color = vec3(1.0, 1.0, 1.0);
    } else if (intensity > 0.95) {
        discrete_color = vertex_color;
    } else if (intensity > 0.5) {
        discrete_color = vertex_color * 0.7;
    } else if (intensity > 0.05) {
        discrete_color = vertex_color * 0.35;
    } else {
        discrete_color = vertex_color * 0.2;
    }

    // mix in texture color if required
    if (useTexture != 0) discrete_color *= texture2D(texture0, uv.st).rgb;

    // set pixel color in OpenGL
    gl_FragColor = vec4(discrete_color, 1.0);
}
