#if 0
#pragma once

#include <volk.h>

#include "bul/containers/handle.h"

#include "camera.h"
#include "gltf.h"
// #include "device.h"

class Renderer
{
public:
    static Renderer create();
    void destroy();

    void init();
    void resize();
    void render();
    void render_gui();

private:
    uint32_t frame_number = 0;

    gltf::Model model;
    std::vector<bul::handle<vk::Image>> model_images;
    bul::handle<vk::Buffer> model_vertex_buffer;
    bul::handle<vk::Buffer> model_index_buffer;

    Camera camera{};

    VkRect2D scissor;
    VkViewport viewport;

    bul::handle<vk::FrameBuffer> render_target;
    bul::handle<vk::Image> rt_color;
    bul::handle<vk::Image> rt_depth;

    bul::handle<vk::GraphicsProgram> graphics_program;
    bul::handle<vk::ComputeProgram> tonemap_program;

    // vk::RingBuffer global_uniform_buffer;
};
#endif