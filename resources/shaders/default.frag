#version 410

#define ALPHA_CUTOFF 0.5

layout (location = 0) in vec2 v_texCoord0;

layout (location = 0) out vec4 o_fragColor;

uniform sampler2D u_baseColorTexture;
uniform vec4 u_baseColorFactor;

// uniform sampler2D u_normalMap;

void main() {
    vec4 baseColor = texture(u_baseColorTexture, v_texCoord0) * u_baseColorFactor;

    if (baseColor.a < ALPHA_CUTOFF) {
        discard;
    }

    o_fragColor = baseColor;
}
