#pragma once

#include <imgui/imgui.h>

namespace vk
{
struct Context;
struct Device;
struct Surface;
struct Command;

void imgui_init(Context& context, Device& device, Surface& surface);
void imgui_shutdown();
void imgui_new_frame();
void imgui_render();
void imgui_render_draw_data(Command& cmd);
} // namespace vk
