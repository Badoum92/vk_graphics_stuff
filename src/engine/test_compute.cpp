#include "test_compute.h"

#include "camera.h"
#include "globals.h"
#include "vox.h"

#include "vk/vk_pipeline.h"
#include "vk/vk_image.h"
#include "vk/vk_buffer.h"

#include "core/math/matrix.h"
#include "core/input.h"
#include "core/log.h"

struct push_constant
{
    VkDeviceAddress uniform_buffer;
    VkDeviceAddress materials;
    vec2u resolution;
    uint32_t frame;
    uint32_t output_image;
    uint32_t voxels_index;
    uint32_t lod1_index;
    uint32_t accumulator_index;
    uint32_t frame_acc;
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
    test_compute.frame_acc = 0;

    test_compute.compute_shader = _context->create_shader("shaders/spv/test_compute.comp");

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
    image_description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    image_description.usage = vk::image_usage_color_attachment;
    image_description.name = "imgui render target";
    test_compute.render_target.image = _context->create_image(image_description);
    test_compute.render_target.sampler = _context->default_sampler;
    test_compute.render_target.descriptor_index =
        _context->image_descriptor_set.create_image_descriptor(_context, test_compute.render_target.image);
    imgui_add_texture(&test_compute.render_target);

    image_description.name = "accumulator";
    test_compute.accumulator = _context->create_image(image_description);
    test_compute.accumulator_index =
        _context->image_descriptor_set.create_image_descriptor(_context, test_compute.accumulator);

    vox vox;
    // vox_load("resources/voxel/vox/monument/monu7.vox", &vox);
    vox_load("resources/voxel/materials.vox", &vox);
    ASSERT(vox.num_models == 1);

    uint32_t w = (vox.models[0].x + 1) / 2;
    uint32_t h = (vox.models[0].y + 1) / 2;
    uint32_t d = (vox.models[0].z + 1) / 2;
    uint32_t lod1_num_voxels = (vox.models[0].x + 1) * (vox.models[0].y + 1) * (vox.models[0].z + 1) / 8;
    uint8_t* lod1 = (uint8_t*)malloc(lod1_num_voxels);

#define INDEX(X, Y, Z, W, H, D) ((W) * (H) * (Z) + (W) * (Y) + (X))
    for (uint32_t z = 0; z < d; ++z)
    {
        for (uint32_t y = 0; y < h; ++y)
        {
            for (uint32_t x = 0; x < w; ++x)
            {
                bool active = false;
                uint32_t index = 0;
                index = INDEX(x * 2 + 0, y * 2 + 0, z * 2 + 0, vox.models[0].x, vox.models[0].y, vox.models[0].z);
                active |= index < vox.models[0].num_voxels && vox.models[0].voxels[index];
                index = INDEX(x * 2 + 1, y * 2 + 0, z * 2 + 0, vox.models[0].x, vox.models[0].y, vox.models[0].z);
                active |= index < vox.models[0].num_voxels && vox.models[0].voxels[index];
                index = INDEX(x * 2 + 0, y * 2 + 1, z * 2 + 0, vox.models[0].x, vox.models[0].y, vox.models[0].z);
                active |= index < vox.models[0].num_voxels && vox.models[0].voxels[index];
                index = INDEX(x * 2 + 1, y * 2 + 1, z * 2 + 0, vox.models[0].x, vox.models[0].y, vox.models[0].z);
                active |= index < vox.models[0].num_voxels && vox.models[0].voxels[index];
                index = INDEX(x * 2 + 0, y * 2 + 0, z * 2 + 1, vox.models[0].x, vox.models[0].y, vox.models[0].z);
                active |= index < vox.models[0].num_voxels && vox.models[0].voxels[index];
                index = INDEX(x * 2 + 1, y * 2 + 0, z * 2 + 1, vox.models[0].x, vox.models[0].y, vox.models[0].z);
                active |= index < vox.models[0].num_voxels && vox.models[0].voxels[index];
                index = INDEX(x * 2 + 0, y * 2 + 1, z * 2 + 1, vox.models[0].x, vox.models[0].y, vox.models[0].z);
                active |= index < vox.models[0].num_voxels && vox.models[0].voxels[index];
                index = INDEX(x * 2 + 1, y * 2 + 1, z * 2 + 1, vox.models[0].x, vox.models[0].y, vox.models[0].z);
                active |= index < vox.models[0].num_voxels && vox.models[0].voxels[index];
                lod1[INDEX(x, y, z, w, h, d)] = active;
            }
        }
    }
#undef INDEX

    image_description.width = w;
    image_description.height = h;
    image_description.depth = d;
    image_description.format = VK_FORMAT_R8_UINT;
    image_description.type = VK_IMAGE_TYPE_3D;
    image_description.usage = vk::image_usage_storage;
    test_compute.lod1 = _context->create_image(image_description);

    buffer_description.size = lod1_num_voxels;
    buffer_description.usage = vk::transfer_buffer_usage;
    buffer_description.name = "lod1 staging buffer";
    vk::buffer* lod1_staging_buffer = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(vox.materials);
    buffer_description.usage = vk::storage_buffer_usage;
    buffer_description.name = "materials";
    test_compute.materials = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(vox.materials);
    buffer_description.usage = vk::transfer_buffer_usage;
    buffer_description.name = "material staging buffer";
    vk::buffer* material_staging_buffer = _context->create_buffer(buffer_description);

    image_description.width = vox.models[0].x;
    image_description.height = vox.models[0].y;
    image_description.depth = vox.models[0].z;
    image_description.format = VK_FORMAT_R8_UINT;
    image_description.type = VK_IMAGE_TYPE_3D;
    image_description.usage = vk::image_usage_storage;
    test_compute.voxels = _context->create_image(image_description);

    buffer_description.size = vox.models[0].x * vox.models[0].y * vox.models[0].z;
    buffer_description.usage = vk::transfer_buffer_usage;
    buffer_description.name = "voxels staging buffer";
    vk::buffer* voxels_staging_buffer = _context->create_buffer(buffer_description);

    vk::command_buffer* cmd = _context->transfer_commands.get_command_buffer();
    cmd->upload_buffer(test_compute.materials, material_staging_buffer, vox.materials, sizeof(vox.materials));
    cmd->upload_image(test_compute.voxels, voxels_staging_buffer, vox.models[0].voxels,
                      voxels_staging_buffer->description.size);
    cmd->upload_image(test_compute.lod1, lod1_staging_buffer, lod1, lod1_staging_buffer->description.size);
    cmd->barrier(test_compute.voxels, vk::image_usage::compute_shader_read);
    cmd->barrier(test_compute.lod1, vk::image_usage::compute_shader_read);
    _context->submit(cmd);
    _context->wait_idle();
    _context->destroy_buffer(material_staging_buffer);
    _context->destroy_buffer(voxels_staging_buffer);
    _context->destroy_buffer(lod1_staging_buffer);

    vox_unload(&vox);

    test_compute.voxels_index = _context->image_descriptor_set.create_image_descriptor(_context, test_compute.voxels);
    test_compute.lod1_index = _context->image_descriptor_set.create_image_descriptor(_context, test_compute.lod1);

    return test_compute;
}

void test_compute::destroy()
{
    context->destroy_compute_pipeline(compute_pipeline);
    context->destroy_shader(compute_shader);
    context->destroy_buffer(uniform_buffer);
    context->destroy_buffer(materials);
    context->destroy_image(voxels);
    context->destroy_image(lod1);
    context->image_descriptor_set.destroy_descriptor(voxels_index);
    context->image_descriptor_set.destroy_descriptor(lod1_index);
    context->image_descriptor_set.destroy_descriptor(accumulator_index);
    imgui_remove_texture(&render_target);
    context->destroy_image(render_target.image);
    context->destroy_image(accumulator);
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
    frame_acc = 0;

    imgui_remove_texture(&render_target);
    context->image_descriptor_set.destroy_descriptor(render_target.descriptor_index);
    context->destroy_image(render_target.image);

    context->image_descriptor_set.destroy_descriptor(accumulator_index);
    context->destroy_image(accumulator);

    vk::image_description image_description = {};
    image_description.width = width;
    image_description.height = height;
    image_description.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    image_description.usage = vk::image_usage_color_attachment;
    image_description.name = "imgui render target";
    render_target.image = context->create_image(image_description);
    render_target.descriptor_index =
        context->image_descriptor_set.create_image_descriptor(context, render_target.image);
    imgui_add_texture(&render_target);

    image_description.name = "accumulator";
    accumulator = context->create_image(image_description);
    accumulator_index = context->image_descriptor_set.create_image_descriptor(context, accumulator);
}

void test_compute::reload_shaders()
{
    context->wait_idle();
    context->destroy_compute_pipeline(compute_pipeline);
    context->destroy_shader(compute_shader);

    frame_acc = 0;

    compute_shader = context->create_shader("shaders/spv/test_compute.comp");

    vk::compute_pipeline_description compute_pipeline_description = {};
    compute_pipeline_description.shader = compute_shader;
    compute_pipeline_description.push_constant_size = sizeof(push_constant);
    compute_pipeline_description.name = "test compute";
    compute_pipeline = context->create_compute_pipeline(compute_pipeline_description);
}

void test_compute::draw(vk::frame_context* frame_context, camera* camera)
{
    vk::command_buffer* command_buffer = frame_context->command_buffer;

    uniform_buffer_data uniform_buffer_data = {};
    uniform_buffer_data.inv_view_proj = mat4_inverse(camera->proj * camera->view);
    uniform_buffer_data.position = {camera->position.x, camera->position.y, camera->position.z, 0.0f};
    memcpy(uniform_buffer->mapped_data, &uniform_buffer_data, sizeof(uniform_buffer_data));

    if (memcmp(&uniform_buffer_data.inv_view_proj, &prev_inv_view_proj, sizeof(prev_inv_view_proj) != 0))
        frame_acc = 0;
    prev_inv_view_proj = uniform_buffer_data.inv_view_proj;

    push_constant push_constant = {};
    push_constant.uniform_buffer = uniform_buffer->device_address;
    push_constant.materials = materials->device_address;
    push_constant.resolution = {width, height};
    push_constant.frame = g_frame;
    push_constant.output_image = render_target.descriptor_index;
    push_constant.voxels_index = voxels_index;
    push_constant.lod1_index = lod1_index;
    push_constant.accumulator_index = accumulator_index;
    push_constant.frame_acc = frame_acc++;

    command_buffer->barrier(render_target.image, vk::image_usage::compute_shader_read_write);

    command_buffer->bind_compute_pipeline(compute_pipeline);
    command_buffer->bind_descriptor_buffer(compute_pipeline);
    command_buffer->push_constant(compute_pipeline, &push_constant, sizeof(push_constant));
    command_buffer->compute_dispatch((width + 7) / 8, (height + 7) / 8, 1);

    command_buffer->barrier(render_target.image, vk::image_usage::compute_shader_read);
}
