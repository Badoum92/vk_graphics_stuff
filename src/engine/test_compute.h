#pragma once

#include "core/math/matrix.h"

#include "vk/vk_context.h"

#include "imgui.h"

struct camera;

struct test_compute
{
    static test_compute create(vk::context* _context, uint32_t _width, uint32_t _height);
    void destroy();

    void resize(uint32_t _width, uint32_t _height);
    void reload_shaders();
    void draw(vk::frame_context* frame_context, camera* camera);

    uint32_t width;
    uint32_t height;

    vk::shader* compute_shader;
    vk::compute_pipeline* compute_pipeline;
    vk::buffer* uniform_buffer;
    vk::buffer* materials;
    vk::image* voxels;
    vk::image* lod1;
    uint32_t voxels_index;
    uint32_t lod1_index;
    uint32_t frame_acc;

    mat4f prev_inv_view_proj;

    vk::image* accumulator;
    uint32_t accumulator_index;
    imgui_texture render_target;

    vk::context* context;
};
