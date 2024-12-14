#pragma once

#include <volk.h>

#include "bul/containers/handle.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_impl_win32.h"

namespace vk
{
struct context;
struct command_buffer;
struct image;
}

namespace bul
{
struct window;
}

struct imgui_texture
{
    bul::handle<vk::image> image;
    VkDescriptorSet vk_descriptorset;
};

void imgui_init(vk::context* context, bul::window* window);
void imgui_shutdown();

void imgui_begin_frame();
void imgui_end_frame(vk::command_buffer* command_buffer);
