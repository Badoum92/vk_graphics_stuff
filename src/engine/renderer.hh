#pragma once

#include <vulkan/vulkan.h>

#include "device.hh"
#include "surface.hh"
#include "camera.hh"
#include "event.hh"
#include "gltf.hh"

class Renderer
{
public:
    static Renderer create(vk::Device& device, vk::Surface& surface);
    void destroy();

    void init();
    void resize();
    void render();

    static void key_callback(const Event& event, void* object);
    static void cursor_pos_callback(const Event& event, void* object);

private:
    vk::Device* p_device = nullptr;
    vk::Surface* p_surface = nullptr;

    gltf::Model model{};

    Camera camera{};

    VkRect2D scissor;
    VkViewport viewport;

    Handle<vk::FrameBuffer> render_target;
    Handle<vk::Image> rt_color;
    Handle<vk::Image> rt_depth;

    Handle<vk::GraphicsProgram> graphics_program;

    vk::RingBuffer global_uniform_buffer;
};
