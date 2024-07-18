#include "test_renderer.h"

#include "bul/math/math.h"

#include "camera.h"

struct push_constant
{
    VkDeviceAddress vertex_buffer;
    VkDeviceAddress uniform_buffer;
};

struct vertex
{
    bul::vec4f position;
    bul::vec4f color;
    bul::vec2f uv;
    bul::vec2f _padding;
};

struct uniform_buffer_data
{
    bul::mat4f view_proj;
};

test_renderer test_renderer::create(vk::context* _context)
{
    test_renderer test_renderer;

    test_renderer.context = _context;

    test_renderer.vertex_shader = _context->create_shader("shaders/test_triangle.vert.spv");
    test_renderer.fragment_shader = _context->create_shader("shaders/test_triangle.frag.spv");

    vk::image swapchain_image = _context->images.get(_context->surface.images[0]);

    vk::graphics_pipeline_description graphics_pipeline_description = {};
    graphics_pipeline_description.vertex_shader = test_renderer.vertex_shader;
    graphics_pipeline_description.fragment_shader = test_renderer.fragment_shader;
    graphics_pipeline_description.color_attachment_formats.push_back(swapchain_image.full_view.format);
    graphics_pipeline_description.depth_attachment_format = VK_FORMAT_D32_SFLOAT;
    graphics_pipeline_description.push_constant_size = sizeof(push_constant);
    graphics_pipeline_description.name = "test triangle";
    test_renderer.graphics_pipeline_handle = _context->create_graphics_pipeline(graphics_pipeline_description);

    uint32_t indices[] = {
        0,  3,  2,  2,  1,  0,  4,  5,  6,  6,  7,  4,  11, 8,  9,  9,  10, 11,
        12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
    };
    vertex vertices[] = {
        {{-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    };

    vk::buffer_description buffer_description = {};
    buffer_description.size = sizeof(uniform_buffer_data);
    buffer_description.usage = vk::uniform_buffer_usage;
    buffer_description.name = "uniform buffer";
    test_renderer.uniform_buffer_handle = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(indices);
    buffer_description.usage = vk::index_buffer_usage;
    buffer_description.name = "index buffer";
    test_renderer.index_buffer_handle = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(vertices);
    buffer_description.usage = vk::storage_buffer_usage;
    buffer_description.name = "vertex buffer";
    test_renderer.vertex_buffer_handle = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(indices);
    buffer_description.usage = vk::transfer_buffer_usage;
    buffer_description.name = "staging buffer";
    bul::handle<vk::buffer> index_staging_buffer = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(vertices);
    buffer_description.usage = vk::transfer_buffer_usage;
    buffer_description.name = "staging buffer";
    bul::handle<vk::buffer> vertex_staging_buffer = _context->create_buffer(buffer_description);

    vk::image_description image_description = {};
    image_description.width = _context->surface.extent.width;
    image_description.height = _context->surface.extent.height;
    image_description.format = VK_FORMAT_D32_SFLOAT;
    image_description.usage = vk::image_usage_depth_attachment;
    image_description.name = "depth image";
    test_renderer.depth_handle = _context->create_image(image_description);

    vk::command_buffer* cmd = _context->transfer_commands.get_command_buffer();
    cmd->upload_buffer(test_renderer.index_buffer_handle, index_staging_buffer, indices, sizeof(indices));
    cmd->upload_buffer(test_renderer.vertex_buffer_handle, vertex_staging_buffer, vertices, sizeof(vertices));
    cmd->barrier(test_renderer.depth_handle, vk::image_usage::depth_attachment);
    _context->submit(cmd);
    _context->wait_idle();
    _context->destroy_buffer(index_staging_buffer);
    _context->destroy_buffer(vertex_staging_buffer);

    test_renderer.y_rotation_deg = 0;

    return test_renderer;
}

void test_renderer::destroy()
{
    context->destroy_graphics_pipeline(graphics_pipeline_handle);
    context->destroy_shader(fragment_shader);
    context->destroy_shader(vertex_shader);
    context->destroy_buffer(uniform_buffer_handle);
    context->destroy_buffer(index_buffer_handle);
    context->destroy_buffer(vertex_buffer_handle);
    context->destroy_image(depth_handle);
}

void test_renderer::resize()
{
    vk::image_description image_description = {};
    image_description.width = context->surface.extent.width;
    image_description.height = context->surface.extent.height;
    image_description.format = VK_FORMAT_D32_SFLOAT;
    image_description.usage = vk::image_usage_depth_attachment;
    image_description.name = "depth image";
    context->destroy_image(depth_handle);
    depth_handle = context->create_image(image_description);
}

void test_renderer::draw(vk::frame_context* frame_context, camera* camera, float delta_time)
{
    vk::command_buffer* command_buffer = frame_context->graphics_commands.get_command_buffer();

    y_rotation_deg += 36 * delta_time;

    vk::buffer& uniform_buffer = context->buffers.get(uniform_buffer_handle);
    uniform_buffer_data uniform_buffer_data = {};
    uniform_buffer_data.view_proj = camera->proj * camera->view * bul::rotation_y(bul_radians(y_rotation_deg));
    memcpy(uniform_buffer.mapped_data, &uniform_buffer_data, sizeof(uniform_buffer_data));

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = context->surface.extent;
    command_buffer->set_scissor(scissor);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)context->surface.extent.width;
    viewport.height = (float)context->surface.extent.height;
    viewport.minDepth = 1.0f;
    viewport.maxDepth = 0.0f;
    command_buffer->set_viewport(viewport);

    command_buffer->barrier(frame_context->image, vk::image_usage::color_attachment);
    command_buffer->begin_rendering({{frame_context->image}}, {{vk::load_op::clear()}},
                                    depth_handle, vk::load_op::clear());

    push_constant push_constant;
    push_constant.vertex_buffer = context->buffers.get(vertex_buffer_handle).device_address;
    push_constant.uniform_buffer = context->buffers.get(uniform_buffer_handle).device_address;
    command_buffer->push_constant(graphics_pipeline_handle, &push_constant, sizeof(push_constant));

    command_buffer->bind_descriptor_buffer(graphics_pipeline_handle);

    vk::graphics_state graphics_state = vk::graphics_state::create();
    graphics_state.cull_back_faces = false;
    command_buffer->bind_pipeline(graphics_pipeline_handle, graphics_state);

    command_buffer->bind_index_buffer(index_buffer_handle);
    command_buffer->draw_indexed(36);

    command_buffer->end_rendering();

    command_buffer->barrier(frame_context->image, vk::image_usage::present);

    context->submit(command_buffer, frame_context);
}