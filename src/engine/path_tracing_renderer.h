#pragma once

#include <volk.h>

#include "vulkan/fwd.h"
#include "camera.h"
#include "device.h"

class PathTracingRenderer
{
public:
    static PathTracingRenderer create();
    void destroy();

    void init();
    void resize();
    void reload_shaders();
    void render();
    void render_gui();

private:
    bool shaders_need_reload = true;

    uint32_t frame_number = 0;
    float exposure = 1;

    Camera camera{};

    VkRect2D scissor;
    VkViewport viewport;

    bul::Handle<vk::Image> color;
    bul::Handle<vk::Image> color_acc;

    bul::Handle<vk::ComputeProgram> raytracing_program;

    bul::Handle<vk::Image> voxels;
    bul::Handle<vk::Buffer> voxel_materials;

    vk::RingBuffer global_uniform_buffer;
};
