#pragma once

#include <imgui/imgui.h>

#include "handle.hh"

namespace vk
{
struct Context;
struct Device;
struct Surface;
struct FrameBuffer;
struct Command;

void imgui_init(Context& context, Device& device, Surface& surface, const Handle<FrameBuffer>& fb_handle);
void imgui_shutdown();
void imgui_new_frame();
void imgui_render();
void imgui_render_draw_data(Command& cmd);
} // namespace vk
