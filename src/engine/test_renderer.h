#pragma once

#include "vk/context.h"

#include "bul/containers/vector.h"

#include "imgui.h"

struct camera;

struct test_renderer
{
    static test_renderer create(vk::context* _context, uint32_t _width, uint32_t _height);
    void destroy();

    void resize(uint32_t _width, uint32_t _height);
    void draw(vk::frame_context* frame_context, camera* camera);

    uint32_t width;
    uint32_t height;

    bul::handle<vk::shader> vertex_shader;
    bul::handle<vk::shader> fragment_shader;
    bul::handle<vk::graphics_pipeline> graphics_pipeline_handle;
    bul::handle<vk::buffer> uniform_buffer_handle;
    bul::vector<bul::handle<vk::buffer>> index_buffer_handle;
    bul::vector<bul::handle<vk::buffer>> vertex_buffer_handle;
    bul::handle<vk::image> depth_handle;

    imgui_texture render_target;

    float y_rotation_deg;

    vk::context* context;
};
