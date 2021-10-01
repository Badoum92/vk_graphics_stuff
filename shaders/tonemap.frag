layout(set = 1, binding = 0) uniform sampler2D hdr_texture;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 frag_color;

void main()
{
    const float exposure = 1.0;
    const float gamma = 2.2;
    vec3 hdr_color = texture(hdr_texture, uv).rgb;

    vec3 mapped = vec3(1.0) - exp(-hdr_color * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));

    frag_color = vec4(mapped, 1.0);
}
