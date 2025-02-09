#pragma once

#include <volk.h>

#include "bul/containers/handle.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_impl_win32.h"

namespace vk
{
struct context;
struct command_buffer;
struct image;
} // namespace vk

namespace bul
{
struct window;
}

struct imgui_texture
{
    bul::handle<vk::image> image;
    VkDescriptorSet vk_descriptorset;
};

extern ImGuiID imgui_global_dockspace;

void imgui_init(vk::context* context, bul::window* window);
void imgui_shutdown();

void imgui_begin_frame();
void imgui_end_frame(vk::command_buffer* command_buffer);

struct imgui_docknode_split_h;
struct imgui_docknode_split_v;

struct imgui_docknode
{
    static imgui_docknode begin(uint32_t _id);
    static imgui_docknode begin_new(uint32_t _id);
    void end();

    void dock_window(const char* name);
    imgui_docknode_split_h split_h(float a_fRatio = 0.5f);
    imgui_docknode_split_v split_v(float a_fRatio = 0.5f);

    uint32_t id;
};

struct imgui_docknode_split_h
{
    imgui_docknode left;
    imgui_docknode right;
};

struct imgui_docknode_split_v
{
    imgui_docknode up;
    imgui_docknode down;
};
