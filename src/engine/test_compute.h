#pragma once

#include "vk/vk_context.h"

#include "imgui.h"

struct camera;

struct test_compute
{
    static test_compute create(vk::context* _context, uint32_t _width, uint32_t _height);
    void destroy();

    void resize(uint32_t _width, uint32_t _height);
    void draw(vk::frame_context* frame_context, camera* camera);

    uint32_t width;
    uint32_t height;

    vk::shader* compute_shader;
    vk::compute_pipeline* compute_pipeline;
    vk::buffer* uniform_buffer;

    imgui_texture render_target;

    vk::context* context;
};
