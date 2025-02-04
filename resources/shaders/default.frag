#version 410

#define ALPHA_CUTOFF 0.5

#if defined HAS_VEC3_COLORS
layout (location = 0) in vec3 v_color0;
#elif defined HAS_VEC4_COLORS
layout (location = 0) in vec4 v_color0;
#endif
layout (location = 1) in vec2 v_texCoord0;

layout (location = 0) out vec4 o_fragColor;

uniform sampler2D u_baseColorTexture;
uniform vec4 u_baseColorFactor;

void main() {
    vec4 baseColor = texture(u_baseColorTexture, v_texCoord0) * u_baseColorFactor;

#if defined HAS_VEC3_COLORS
    baseColor *= vec4(v_color0, 1.0);
#elif defined HAS_VEC4_COLORS
    baseColor *= v_color0;
#endif

    if (baseColor.a < ALPHA_CUTOFF) {
        discard;
    }

    o_fragColor = baseColor;
}
