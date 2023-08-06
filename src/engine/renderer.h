#pragma once

#include <volk.h>

#include "camera.h"
#include "gltf.h"
#include "device.h"

#include "fwd.h"

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
    std::vector<bul::Handle<vk::Image>> model_images;
    bul::Handle<vk::Buffer> model_vertex_buffer;
    bul::Handle<vk::Buffer> model_index_buffer;

    Camera camera{};

    VkRect2D scissor;
    VkViewport viewport;

    bul::Handle<vk::FrameBuffer> render_target;
    bul::Handle<vk::Image> rt_color;
    bul::Handle<vk::Image> rt_depth;

    bul::Handle<vk::GraphicsProgram> graphics_program;
    bul::Handle<vk::ComputeProgram> tonemap_program;

    vk::RingBuffer global_uniform_buffer;
};
