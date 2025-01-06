#include "test_compute.h"

#include "camera.h"

#include "bul/math/matrix.h"
#include "bul/log.h"

struct push_constant
{
    VkDeviceAddress uniform_buffer;
    uint32_t render_target_descriptor_index;
};

struct uniform_buffer_data
{
    bul::mat4f view_proj;
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
    test_compute.compute_pipeline_handle = _context->create_compute_pipeline(compute_pipeline_description);

    vk::buffer_description buffer_description = {};
    buffer_description.size = sizeof(uniform_buffer_data);
    buffer_description.usage = vk::uniform_buffer_usage;
    buffer_description.name = "uniform buffer";
    test_compute.uniform_buffer_handle = _context->create_buffer(buffer_description);

    vk::image_description image_description = {};
    image_description.width = test_compute.width;
    image_description.height = test_compute.height;
    image_description.format = _context->images.get(_context->surface.images[0]).description.format;
    image_description.usage = vk::image_usage_color_attachment;
    image_description.name = "imgui render target";
    test_compute.render_target.image = _context->create_image(image_description);

    test_compute.render_target_descriptor_index = _context->descriptor_set.create_descriptor(
        _context, test_compute.render_target.image, {}, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

    vk::image& render_target_image = _context->images.get(test_compute.render_target.image);
    vk::sampler& render_target_sampler = _context->samplers.get(_context->default_sampler);
    test_compute.render_target.vk_descriptorset =
        ImGui_ImplVulkan_AddTexture(render_target_sampler.vk_handle, render_target_image.full_view.vk_handle,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    return test_compute;
}

void test_compute::destroy()
{
    context->destroy_compute_pipeline(compute_pipeline_handle);
    context->destroy_shader(compute_shader);
    context->destroy_buffer(uniform_buffer_handle);
    ImGui_ImplVulkan_RemoveTexture(render_target.vk_descriptorset);
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

    ImGui_ImplVulkan_RemoveTexture(render_target.vk_descriptorset);
    context->descriptor_set.destroy_descriptor(render_target_descriptor_index);
    context->destroy_image(render_target.image);

    vk::image_description image_description = {};
    image_description.width = width;
    image_description.height = height;
    image_description.format = context->images.get(context->surface.images[0]).description.format;
    image_description.usage = vk::image_usage_color_attachment;
    image_description.name = "imgui render target";
    render_target.image = context->create_image(image_description);

    render_target_descriptor_index =
        context->descriptor_set.create_descriptor(context, render_target.image, {}, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);

    vk::image& render_target_image = context->images.get(render_target.image);
    vk::sampler& render_target_sampler = context->samplers.get(context->default_sampler);
    render_target.vk_descriptorset =
        ImGui_ImplVulkan_AddTexture(render_target_sampler.vk_handle, render_target_image.full_view.vk_handle,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void test_compute::draw(vk::frame_context* frame_context, camera* camera)
{
    vk::command_buffer* command_buffer = frame_context->command_buffer;

    vk::buffer& uniform_buffer = context->buffers.get(uniform_buffer_handle);
    uniform_buffer_data uniform_buffer_data = {};
    uniform_buffer_data.view_proj = camera->proj * camera->view;
    memcpy(uniform_buffer.mapped_data, &uniform_buffer_data, sizeof(uniform_buffer_data));

    push_constant push_constant = {};
    push_constant.uniform_buffer = context->buffers.get(uniform_buffer_handle).device_address;
    push_constant.render_target_descriptor_index = render_target_descriptor_index;

    command_buffer->barrier(render_target.image, vk::image_usage::compute_shader_read_write);

    command_buffer->bind_compute_pipeline(compute_pipeline_handle);
    command_buffer->bind_descriptor_buffer(compute_pipeline_handle);
    command_buffer->push_constant(compute_pipeline_handle, &push_constant, sizeof(push_constant));
    command_buffer->compute_dispatch(width, height, 1);

    command_buffer->barrier(render_target.image, vk::image_usage::compute_shader_read);
}
