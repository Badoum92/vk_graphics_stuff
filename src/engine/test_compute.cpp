#include "test_compute.h"

#include "camera.h"
#include "globals.h"

#include "vk/vk_pipeline.h"
#include "vk/vk_image.h"
#include "vk/vk_buffer.h"

#include "core/math/matrix.h"
#include "core/log.h"

struct push_constant
{
    VkDeviceAddress uniform_buffer;
    vec2u resolution;
    uint32_t frame;
    uint32_t image_index;
};

struct uniform_buffer_data
{
    mat4f inv_view_proj;
    vec4f position;
};

test_compute test_compute::create(vk::context* _context, uint32_t _width, uint32_t _height)
{
    test_compute test_compute;

    test_compute.width = _width;
    test_compute.height = _height;
    test_compute.context = _context;

    test_compute.compute_shader = _context->create_shader("shaders/test_compute.comp.spv");

    vk::compute_pipeline_description compute_pipeline_description = {};
    compute_pipeline_description.shader = test_compute.compute_shader;
    compute_pipeline_description.push_constant_size = sizeof(push_constant);
    compute_pipeline_description.name = "test compute";
    test_compute.compute_pipeline = _context->create_compute_pipeline(compute_pipeline_description);

    vk::buffer_description buffer_description = {};
    buffer_description.size = sizeof(uniform_buffer_data);
    buffer_description.usage = vk::uniform_buffer_usage;
    buffer_description.name = "uniform buffer";
    test_compute.uniform_buffer = _context->create_buffer(buffer_description);

    vk::image_description image_description = {};
    image_description.width = test_compute.width;
    image_description.height = test_compute.height;
    image_description.format = _context->surface.images[0]->description.format;
    image_description.usage = vk::image_usage_color_attachment;
    image_description.name = "imgui render target";
    test_compute.render_target.image = _context->create_image(image_description);
    test_compute.render_target.sampler = _context->default_sampler;
    test_compute.render_target.descriptor_index =
        _context->image_descriptor_set.create_image_descriptor(_context, test_compute.render_target.image);
    imgui_add_texture(&test_compute.render_target);

    return test_compute;
}

void test_compute::destroy()
{
    context->destroy_compute_pipeline(compute_pipeline);
    context->destroy_shader(compute_shader);
    context->destroy_buffer(uniform_buffer);
    imgui_remove_texture(&render_target);
    context->destroy_image(render_target.image);
}

void test_compute::resize(uint32_t _width, uint32_t _height)
{
    if (_width == width && _height == height)
    {
        return;
    }

    context->wait_idle();

    width = _width;
    height = _height;

    imgui_remove_texture(&render_target);
    context->image_descriptor_set.destroy_descriptor(render_target.descriptor_index);
    context->destroy_image(render_target.image);

    vk::image_description image_description = {};
    image_description.width = width;
    image_description.height = height;
    image_description.format = context->surface.images[0]->description.format;
    image_description.usage = vk::image_usage_color_attachment;
    image_description.name = "imgui render target";
    render_target.image = context->create_image(image_description);
    render_target.descriptor_index =
        context->image_descriptor_set.create_image_descriptor(context, render_target.image);
    imgui_add_texture(&render_target);
}

void test_compute::draw(vk::frame_context* frame_context, camera* camera)
{
    vk::command_buffer* command_buffer = frame_context->command_buffer;

    uniform_buffer_data uniform_buffer_data = {};
    uniform_buffer_data.inv_view_proj = mat4_inverse(camera->proj * camera->view);
    uniform_buffer_data.position = {camera->position.x, camera->position.y, camera->position.z, 0.0f};
    memcpy(uniform_buffer->mapped_data, &uniform_buffer_data, sizeof(uniform_buffer_data));

    push_constant push_constant = {};
    push_constant.uniform_buffer = uniform_buffer->device_address;
    push_constant.resolution = {width, height};
    push_constant.frame = g_frame;
    push_constant.image_index = render_target.descriptor_index;

    command_buffer->barrier(render_target.image, vk::image_usage::compute_shader_read_write);

    command_buffer->bind_compute_pipeline(compute_pipeline);
    command_buffer->bind_descriptor_buffer(compute_pipeline);
    command_buffer->push_constant(compute_pipeline, &push_constant, sizeof(push_constant));
    command_buffer->compute_dispatch((width + 7) / 8, (height + 7) / 8, 1);

    command_buffer->barrier(render_target.image, vk::image_usage::compute_shader_read);
}
