#pragma once

#include "vk/context.h"

struct camera;

struct test_renderer
{
    static test_renderer create(vk::context* _context);
    void destroy();

    void resize();
    void draw(vk::frame_context* frame_context, camera* camera, float delta_time);

    bul::handle<vk::shader> vertex_shader;
    bul::handle<vk::shader> fragment_shader;
    bul::handle<vk::graphics_pipeline> graphics_pipeline_handle;
    bul::handle<vk::buffer> uniform_buffer_handle;
    bul::handle<vk::buffer> index_buffer_handle;
    bul::handle<vk::buffer> vertex_buffer_handle;
    bul::handle<vk::image> depth_handle;

    float y_rotation_deg;

    vk::context* context;
};