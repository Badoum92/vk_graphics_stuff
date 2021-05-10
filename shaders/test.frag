#include "global.glsl"

layout(location = 0) in vec2 in_tex_coord;

layout(location = 0) out vec4 out_color;

layout(set = 1, binding = 1) uniform sampler2D image;

void main()
{
    out_color = texture(image, in_tex_coord);
}
