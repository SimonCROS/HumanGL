#version 410

#define ALPHA_CUTOFF 0.5

layout (location = 0) in vec3 v_position;
#if defined HAS_VEC3_COLORS
layout (location = 1) in vec3 v_color0;
#elif defined HAS_VEC4_COLORS
layout (location = 1) in vec4 v_color0;
#endif
layout (location = 2) in vec3 v_normal;
layout (location = 3) in vec2 v_texCoord0;

layout (location = 0) out vec4 o_fragColor;

uniform sampler2D u_baseColorTexture;
uniform vec4 u_baseColorFactor;

const vec3 c_lightPos = vec3(-500, 500, -250);
const vec3 c_lightColor = vec3(1, 1, 1);
const vec3 c_ambientColor = vec3(1, 1, 1);
const float c_ambientFactor = 0.5f;

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

    vec3 norm = gl_FrontFacing ? normalize(v_normal) : -normalize(v_normal);
    vec3 lightDir = normalize(c_lightPos - v_position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * c_lightColor;

    o_fragColor = vec4(((c_ambientColor * c_ambientFactor) + diffuse), 1) * baseColor;
}
