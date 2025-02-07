#version 410

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
#if defined HAS_VEC3_COLORS
layout (location = 2) in vec3 a_color0;
#elif defined HAS_VEC4_COLORS
layout (location = 2) in vec4 a_color0;
#endif
layout (location = 3) in vec2 a_texCoord0;

layout (location = 0) out vec3 v_position;
#if defined HAS_VEC3_COLORS
layout (location = 1) out vec3 v_color0;
#elif defined HAS_VEC4_COLORS
layout (location = 1) out vec4 v_color0;
#endif
layout (location = 2) out vec3 v_normal;
layout (location = 3) out vec2 v_texCoord0;

uniform mat4 u_projectionView;
uniform mat4 u_transform;

void main() {
    gl_Position = u_projectionView * u_transform * vec4(a_position, 1.0f);
    v_position = gl_Position.xyz;

#if defined HAS_VEC3_COLORS || defined HAS_VEC4_COLORS
    v_color0 = a_color0;
#endif

    mat3 normalMatrix = transpose(inverse(mat3(u_transform))); // TODO pass normal matrix as argument ?
    v_normal = normalize(normalMatrix * a_normal);

    v_texCoord0 = a_texCoord0;
}
