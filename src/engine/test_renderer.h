#pragma once

#include "vk/vk_context.h"

#include "core/containers/vector.h"

#include "imgui.h"

struct camera;

struct test_renderer
{
    static test_renderer create(vk::context* _context, uint32_t _width, uint32_t _height);
    void destroy();

    void resize(uint32_t _width, uint32_t _height);
    void reload_shaders();
    void draw(vk::frame_context* frame_context, camera* camera);

    uint32_t width;
    uint32_t height;

    vk::shader* vertex_shader;
    vk::shader* fragment_shader;
    vk::graphics_pipeline* graphics_pipeline;
    vk::buffer* uniform_buffer;
    bul::vector<vk::buffer*> index_buffers;
    bul::vector<vk::buffer*> vertex_buffers;
    vk::image* depth;

    imgui_texture render_target;

    vk::context* context;
};
