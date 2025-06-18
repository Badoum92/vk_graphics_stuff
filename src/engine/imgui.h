#pragma once

#include <volk.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace vk
{
struct context;
struct command_buffer;
struct image;
struct sampler;
} // namespace vk

struct window;

struct imgui_texture
{
    vk::image* image;
    vk::sampler* sampler;
    VkDescriptorSet vk_descriptorset;
    uint32_t descriptor_index;
};

extern ImGuiID imgui_global_dockspace;

void imgui_init(vk::context* context, window* window);
void imgui_shutdown();

void imgui_begin_frame();
void imgui_end_frame(vk::command_buffer* command_buffer);

void imgui_add_texture(imgui_texture* texture);
void imgui_remove_texture(imgui_texture* texture);

struct imgui_docknode_split
{
    ImGuiID first;
    ImGuiID second;
};

ImGuiID imgui_docknode_main();
void imgui_docknode_begin(ImGuiID id);
void imgui_docknode_end(ImGuiID id);
imgui_docknode_split imgui_docknode_split_v(ImGuiID id, float ratio = 0.5f);
imgui_docknode_split imgui_docknode_split_h(ImGuiID id, float ratio = 0.5f);
void imgui_docknode_window(ImGuiID id, const char* name);