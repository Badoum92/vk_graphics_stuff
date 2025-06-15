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

    bul::handle<vk::shader> compute_shader;
    bul::handle<vk::compute_pipeline> compute_pipeline_handle;
    bul::handle<vk::buffer> uniform_buffer_handle;

    imgui_texture render_target;
    uint32_t render_target_descriptor_index;

    vk::context* context;
};
