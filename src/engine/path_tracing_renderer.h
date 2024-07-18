#if 0
#pragma once

#include <volk.h>

#include "bul/containers/handle.h"

#include "vulkan/fwd.h"
#include "camera.h"
// #include "device.h"

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

    bul::handle<vk::Image> color;
    bul::handle<vk::Image> color_acc;

    bul::handle<vk::ComputeProgram> raytracing_program;

    bul::handle<vk::Image> voxels;
    bul::handle<vk::Buffer> voxel_materials;

    // vk::RingBuffer global_uniform_buffer;
};
#endif