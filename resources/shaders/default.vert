#version 410

#define NUM_JOINTS 42

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoord0;

layout (location = 0) out vec2 v_texCoord0;

uniform mat4 u_projectionView;
uniform mat4 u_transform;
//uniform mat4 u_jointMatrix[NUM_JOINTS];

void main() {
//    mat4 skinMatrix =
//        a_weight.x * u_jointMatrix[int(a_joint.x)] +
//        a_weight.y * u_jointMatrix[int(a_joint.y)] +
//        a_weight.z * u_jointMatrix[int(a_joint.z)] +
//        a_weight.w * u_jointMatrix[int(a_joint.w)];
//
//    gl_Position = u_projectionView * u_transform * skinMatrix * vec4(a_position, 1.0f);

    gl_Position = u_projectionView * u_transform * vec4(a_position, 1.0f);
    v_texCoord0 = a_texCoord0;
}
