#pragma once

#include <volk.h>

#include "vulkan/fwd.h"
#include "camera.h"
#include "device.h"

class PathTracingRenderer
{
public:
    static PathTracingRenderer create(vk::Context& context, vk::Device& device, vk::Surface& surface);
    void destroy();

    void init();
    void resize();
    void reload_shaders();
    void render();
    void render_gui();

private:
    vk::Context* p_context = nullptr;
    vk::Device* p_device = nullptr;
    vk::Surface* p_surface = nullptr;

    bool shaders_need_reload = true;

    uint32_t frame_number = 0;
    float exposure = 1;

    Camera camera{};

    VkRect2D scissor;
    VkViewport viewport;

    Handle<vk::Image> color;
    Handle<vk::Image> color_acc;

    Handle<vk::ComputeProgram> raytracing_program;

    Handle<vk::Image> voxels;
    Handle<vk::Buffer> voxel_materials;

    vk::RingBuffer global_uniform_buffer;
};
