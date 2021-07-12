#include "renderer.hh"

#include <iostream>
#include <glm/glm.hpp>

#include "window.hh"
#include "input.hh"

struct GlobalUniformSet
{
    glm::mat4 view;
    glm::mat4 inv_view;
    glm::mat4 proj;
    glm::mat4 inv_proj;
    glm::vec4 camera_pos;
    glm::uvec2 resolution;
};

static glm::vec4 vertices[6] = {glm::vec4(-0.5, -0.5, -1.0, 1.0), glm::vec4(0.5, -0.5, -1.0, 1.0),
                                glm::vec4(-0.5, 0.5, -1.0, 1.0),  glm::vec4(-0.5, 0.5, -1.0, 1.0),
                                glm::vec4(0.5, -0.5, -1.0, 1.0),  glm::vec4(0.5, 0.5, -1.0, 1.0)};

static glm::vec4 colors[6] = {glm::vec4(1.0, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 0.0),
                              glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(1.0, 0.0, 0.0, 0.0),
                              glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 0.0)};

Renderer Renderer::create(vk::Device& device, vk::Surface& surface)
{
    Renderer renderer;
    renderer.p_device = &device;
    renderer.p_surface = &surface;
    return renderer;
}

void Renderer::destroy()
{}

void Renderer::init()
{
    EventHandler::register_key_callback(this);
    EventHandler::register_cursor_pos_callback(this);

    camera.set_speed(1.0f);

    scissor.offset = {0, 0};
    scissor.extent = {Window::width(), Window::height()};
    viewport.width = scissor.extent.width;
    viewport.height = scissor.extent.height;
    viewport.x = 0;
    viewport.y = 0;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    {
        const auto& fb_desc = p_device->framebuffers.get(p_surface->frame_buffers[0]).description;

        vk::GraphicsProgramDescription prog_desc{};
        prog_desc.attachment_formats = fb_desc;
        prog_desc.descriptor_types = {vk::DescriptorType::create(vk::DescriptorType::Type::StorageBuffer),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::StorageBuffer),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::SampledImage)};
        prog_desc.vertex_shader = p_device->create_shader("shaders/test.vert");
        prog_desc.fragment_shader = p_device->create_shader("shaders/test.frag");
        graphics_program = p_device->create_graphics_program(prog_desc);

        vk::RenderState render_state{};
        render_state.depth.test_enable = VK_FALSE;
        render_state.depth.write_enable = VK_FALSE;

        p_device->compile(graphics_program, render_state);
    }
    {
        vertex_buffer =
            p_device->create_buffer({.size = 6 * sizeof(glm::vec4),
                                     .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                     .memory_usage = VMA_MEMORY_USAGE_GPU_ONLY});

        color_buffer =
            p_device->create_buffer({.size = 6 * sizeof(glm::vec4),
                                     .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                     .memory_usage = VMA_MEMORY_USAGE_GPU_ONLY});

        auto& cmd = p_device->get_transfer_command();
        cmd.upload_buffer(vertex_buffer, vertices, 6 * sizeof(glm::vec4));
        cmd.upload_buffer(color_buffer, colors, 6 * sizeof(glm::vec4));

        image = p_device->create_image({}, "skelesanta.png");
        cmd.upload_image(image, "skelesanta.png");

        p_device->submit_blocking(cmd);
    }
    {
        global_uniform_buffer =
            vk::RingBuffer::create<GlobalUniformSet>(*p_device,
                                                     {.size = p_device->MAX_FRAMES * sizeof(GlobalUniformSet),
                                                      .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                      .memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU});
    }
}

void Renderer::resize()
{
    p_device->wait_idle();

    scissor.extent = {Window::width(), Window::height()};
    viewport.width = scissor.extent.width;
    viewport.height = scissor.extent.height;

    p_surface->destroy_swapchain(*p_device);
    p_surface->create_swapchain(*p_device);
}

void Renderer::render()
{
    if (!p_device->acquire_next_image(*p_surface))
    {
        resize();
        return;
    }

    camera.update();

    auto& fc = p_device->frame_context();
    auto& cmd = p_device->get_graphics_command();

    cmd.set_scissor(scissor);
    cmd.set_viewport(viewport);

    GlobalUniformSet global_uniform_set{};
    global_uniform_set.view = camera.get_view();
    global_uniform_set.inv_view = camera.get_inv_view();
    global_uniform_set.proj = camera.get_proj();
    global_uniform_set.inv_proj = camera.get_inv_proj();
    global_uniform_set.camera_pos = glm::vec4(camera.get_pos(), 1.0f);
    global_uniform_set.resolution = {Window::width(), Window::height()};
    uint32_t offset = global_uniform_buffer.push(global_uniform_set);
    p_device->global_uniform_set.bind_uniform_buffer(0, global_uniform_buffer.buffer_handle, offset,
                                                     sizeof(GlobalUniformSet));
    cmd.bind_descriptor_set(graphics_program, p_device->global_uniform_set, 0);

    cmd.bind_storage_buffer(graphics_program, vertex_buffer, 0);
    cmd.bind_storage_buffer(graphics_program, color_buffer, 1);
    cmd.barrier(image, vk::ImageUsage::GraphicsShaderRead);
    cmd.bind_image(graphics_program, image, 2);
    cmd.bind_pipeline(graphics_program);

    cmd.barrier(fc.image, vk::ImageUsage::ColorAttachment);

    cmd.begin_renderpass(fc.framebuffer);
    cmd.draw(6);
    cmd.end_renderpass();

    cmd.barrier(fc.image, vk::ImageUsage::Present);

    p_device->submit(cmd, fc.image_acquired_semaphore, fc.rendering_finished_semaphore, fc.rendering_finished_fence);

    if (!p_device->present(*p_surface))
    {
        resize();
    }
}

void Renderer::key_callback(const Event& event, void* object)
{
    auto& renderer = *reinterpret_cast<Renderer*>(object);
    if (event.key() == GLFW_KEY_LEFT_ALT && event.key_action() == GLFW_PRESS)
    {
        Input::show_cursor(!Input::cursor_enabled());
        auto [x, y] = Input::get_cursor_pos();
        renderer.camera.set_last_x_y(x, y);
        return;
    }
    renderer.camera.on_key_event(event.key(), event.key_action());
}

void Renderer::cursor_pos_callback(const Event& event, void* object)
{
    auto& renderer = *reinterpret_cast<Renderer*>(object);
    if (!Input::cursor_enabled())
    {
        renderer.camera.on_mouse_moved(event.pos_x(), event.pos_y());
    }
}
