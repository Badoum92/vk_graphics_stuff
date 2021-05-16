#pragma once

#include <imgui/imgui.h>
#include <string>
#include <functional>

#include "command_buffer/command_buffer.hh"

namespace VkImgui
{
    void init();
    void shutdown();

    void new_frame();
    void render();
    void render_draw_data(const CommandBuffer& cmd);

    void window(const std::string& name, const std::function<void()>& f);
} // namespace VkImgui
