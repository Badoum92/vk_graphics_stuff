#include "test.glsl"

/* layout(set = 1, binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 position;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(fragColor * texture(texSampler, position).rgb, 1.0);
} */

layout(set = 1, binding = 2) uniform sampler2D base_color_tex;

layout(location = 0) in vec4 world_pos;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 uv_0;

layout(location = 0) out vec4 frag_color;

void main()
{
    vec3 light_pos = vec3(0, 10, 0);
    vec3 light_dir = normalize(light_pos - world_pos.xyz);
    float diffuse = max(dot(normal.xyz, light_dir), 0.0);
    float ambiant = 0.01;

    frag_color = texture(base_color_tex, uv_0) * (diffuse + ambiant);
}
