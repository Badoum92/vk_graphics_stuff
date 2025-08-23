#pragma once

#include "core/math/vector.h"
#include "core/color.h"

struct camera;

namespace vk
{
struct context;
struct image;
struct command_buffer;
} // namespace vk

void debug_draw_init(vk::context* vk_context);
void debug_draw_shutdown(vk::context* vk_context);
void debug_draw_render(camera* camera, vk::context* vk_context, vk::command_buffer* cmd, vk::image* color,
                       vk::image* depth);

void debug_draw_set_line_width(uint32_t width);

void debug_draw_line_2d(vec2f a, vec2f b, color color);
void debug_draw_triangle_2d(vec2f a, vec2f b, vec2f c, color color);
void debug_draw_aabox_2d_full(vec2f min, vec2f max, color color);
void debug_draw_aabox_2d_wire(vec2f min, vec2f max, color color);

void debug_draw_line_3d(vec3f a, vec3f b, color color);
void debug_draw_triangle_3d(vec3f a, vec3f b, vec3f c, color color);

void debug_draw_aabox_wire(vec3f a, vec3f b, color color);
void debug_draw_aabox_full(vec3f a, vec3f b, color color);