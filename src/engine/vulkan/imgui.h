#pragma once

#include <imgui/imgui.h>

#include "fwd.h"

namespace vk
{
void imgui_init();
void imgui_shutdown();
void imgui_new_frame();
void imgui_render();
void imgui_render_draw_data(Command& cmd);
} // namespace vk
