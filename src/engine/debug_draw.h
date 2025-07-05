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
void debug_draw_render(vk::context* vk_context, vk::command_buffer* cmd, vk::image* render_target, camera* camera);

void debug_draw_set_line_width(uint32_t width);

void debug_draw_line_3d(vec3f a, vec3f b, color color);
void debug_draw_triangle_3d(vec3f a, vec3f b, vec3f c, color color);

void debug_draw_aabb_wire(vec3f a, vec3f b, color color);
void debug_draw_aabb_full(vec3f a, vec3f b, color color);