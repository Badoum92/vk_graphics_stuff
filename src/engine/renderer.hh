#pragma once

#include <vulkan/vulkan.h>

#include "device.hh"
#include "surface.hh"
#include "camera.hh"
#include "event.hh"

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

    Camera camera{};

    VkRect2D scissor;
    VkViewport viewport;

    Handle<vk::GraphicsProgram> graphics_program;
    Handle<vk::Buffer> vertex_buffer;
    Handle<vk::Buffer> color_buffer;
    Handle<vk::Image> image;

    vk::RingBuffer global_uniform_buffer;
};
