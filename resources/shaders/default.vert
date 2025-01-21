#version 410

layout(location = 0) in vec3 vertPosition;
layout(location = 3) in vec3 vertColor;

out vec3 fragColor;

uniform mat4 projectionView;
uniform mat4 transform;

void main() {
    gl_Position = projectionView * transform * vec4(vertPosition, 1.0f);
    fragColor = vertColor;
}
