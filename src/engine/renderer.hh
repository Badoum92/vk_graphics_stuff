#pragma once

#include <vulkan/vulkan.h>

#include "camera.hh"
#include "event.hh"
#include "gltf.hh"

namespace vk
{
struct Context;
struct Device;
struct Surface;
} // namespace vk

class Renderer
{
public:
    static Renderer create(vk::Context& context, vk::Device& device, vk::Surface& surface);
    void destroy();

    void init();
    void resize();
    void render();
    void render_imgui();

    static void key_callback(const Event& event, void* object);
    static void cursor_pos_callback(const Event& event, void* object);

private:
    vk::Context* p_context = nullptr;
    vk::Device* p_device = nullptr;
    vk::Surface* p_surface = nullptr;

    gltf::Model model{};

    Camera camera{};

    VkRect2D scissor;
    VkViewport viewport;

    Handle<vk::FrameBuffer> render_target;
    Handle<vk::Image> rt_color;
    Handle<vk::Image> rt_depth;

    Handle<vk::FrameBuffer> tonemap_fb;
    Handle<vk::Image> tonemap_image;

    Handle<vk::GraphicsProgram> graphics_program;
    Handle<vk::GraphicsProgram> tonemap_program;

    vk::RingBuffer global_uniform_buffer;
};
