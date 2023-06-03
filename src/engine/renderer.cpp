#include "renderer.h"

#include <iostream>
#include <bul/math/matrix.h>
#include <bul/time.h>
#include <bul/window.h>

#include "context.h"
#include "device.h"
#include "surface.h"
#include "imgui.h"

struct GlobalUniformSet
{
    bul::mat4f view;
    bul::mat4f inv_view;
    bul::mat4f proj;
    bul::mat4f inv_proj;
    bul::vec4f camera_pos;
    bul::vec2u resolution;
    float exposure;
    uint32_t frame_number;
};

Renderer Renderer::create(vk::Context& context, vk::Device& device, vk::Surface& surface)
{
    Renderer renderer;
    renderer.p_context = &context;
    renderer.p_device = &device;
    renderer.p_surface = &surface;
    return renderer;
}

void Renderer::destroy()
{
    vk::imgui_shutdown();
}

void Renderer::init()
{
    resize();

    vk::imgui_init(*p_context, *p_device, *p_surface);

    camera.set_speed(50.0f);

    auto& cmd = p_device->get_graphics_command();
    {
        auto& transfer_cmd = p_device->get_transfer_command();
        model = gltf::load("../models/Sponza/glTF/Sponza.gltf");
        // model = gltf::load("../models/backpack/scene.gltf");
        model_vertex_buffer =
            p_device->create_buffer({.size = (uint32_t)(model.vertices.size() * sizeof(gltf::Vertex))});
        model_index_buffer =
            p_device->create_buffer({.size = (uint32_t)(model.indices.size() * sizeof(uint32_t)),
                                     .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT});

        transfer_cmd.upload_buffer(model_vertex_buffer, model.vertices.data(), model.vertices.size() * sizeof(gltf::Vertex));
        transfer_cmd.upload_buffer(model_index_buffer, model.indices.data(), model.indices.size() * sizeof(uint32_t));

        model_images.reserve(model.images.size());
        for (const auto& image : model.images)
        {
            model_images.push_back(p_device->create_image({}, image.uri));
            transfer_cmd.upload_image(model_images.back(), image.uri);
        }

        p_device->submit_blocking(transfer_cmd);

        for (auto& image : model_images)
        {
            cmd.barrier(image, vk::ImageUsage::GraphicsShaderRead);
        }
    }
    {
        vk::GraphicsProgramDescription prog_desc{};
        prog_desc.attachment_formats = p_device->framebuffers.get(render_target).description;
        prog_desc.descriptor_types = {vk::DescriptorType::create(vk::DescriptorType::Type::StorageBuffer),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::DynamicBuffer),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::SampledImage)};
        prog_desc.vertex_shader = p_device->create_shader("shaders/test.vert");
        prog_desc.fragment_shader = p_device->create_shader("shaders/test.frag");
        graphics_program = p_device->create_graphics_program(prog_desc);

        vk::RenderState render_state{};
        render_state.depth.test_enable = VK_TRUE;
        render_state.depth.write_enable = VK_TRUE;

        p_device->compile(graphics_program, render_state);
    }
    {
        vk::ComputeProgramDescription prog_desc{};
        prog_desc.descriptor_types = {vk::DescriptorType::create(vk::DescriptorType::Type::SampledImage),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::StorageImage)};
        prog_desc.shader = p_device->create_shader("shaders/tonemap.comp");
        tonemap_program = p_device->create_compute_program(prog_desc);
    }
    {
        global_uniform_buffer = vk::RingBuffer::create(
            *p_device,
            {.size = 4 * MB, .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, .memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU});
    }
    p_device->submit_blocking(cmd);
}

void Renderer::resize()
{
    p_device->wait_idle();
    frame_number = 0;

    scissor.offset = {0, 0};
    scissor.extent = {bul::window::size().x, bul::window::size().y};
    viewport.width = scissor.extent.width;
    viewport.height = scissor.extent.height;
    viewport.x = 0;
    viewport.y = 0;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    p_surface->destroy_swapchain(*p_device);
    p_surface->create_swapchain(*p_device);

    const auto& fb_desc = p_device->framebuffers.get(p_surface->framebuffers[0]).description;

    {
        p_device->destroy_image(p_device->images.get(rt_color));
        p_device->images.erase(rt_color);
        p_device->destroy_image(p_device->images.get(rt_depth));
        p_device->images.erase(rt_depth);
        p_device->destroy_framebuffer(p_device->framebuffers.get(render_target));
        p_device->framebuffers.erase(render_target);

        rt_color = p_device->create_image({.width = fb_desc.width,
                                           .height = fb_desc.height,
                                           .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                                           .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
                                               | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT});

        rt_depth = p_device->create_image({.width = fb_desc.width,
                                           .height = fb_desc.height,
                                           .format = VK_FORMAT_D32_SFLOAT,
                                           .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT});

        render_target =
            p_device->create_framebuffer({.width = fb_desc.width,
                                          .height = fb_desc.height,
                                          .color_formats = {p_device->images.get(rt_color).description.format},
                                          .depth_format = {p_device->images.get(rt_depth).description.format}},
                                         {rt_color}, rt_depth);
    }
}

void Renderer::render()
{
    if (!p_device->acquire_next_image(*p_surface))
    {
        resize();
        return;
    }

    camera.update();
    if (camera.is_moving())
    {
        frame_number = 0;
    }

    auto& fc = p_device->frame_context();
    auto& cmd = p_device->get_graphics_command();

    cmd.set_scissor(scissor);
    cmd.set_viewport(viewport);

    // global set

    GlobalUniformSet global_uniform_set{};
    global_uniform_set.view = camera.get_view();
    global_uniform_set.inv_view = camera.get_inv_view();
    global_uniform_set.proj = camera.get_proj();
    global_uniform_set.inv_proj = camera.get_inv_proj();
    global_uniform_set.camera_pos = bul::vec4f(camera.get_pos(), 1.0f);
    global_uniform_set.resolution = {bul::window::size().x, bul::window::size().y};
    global_uniform_set.frame_number = frame_number;
    uint32_t uniform_offset = global_uniform_buffer.push(&global_uniform_set, sizeof(GlobalUniformSet));
    p_device->global_uniform_set.bind_uniform_buffer(0, global_uniform_buffer.buffer_handle, uniform_offset,
                                                     sizeof(GlobalUniformSet));
    cmd.bind_descriptor_set(graphics_program, p_device->global_uniform_set, 0);

    // main renderpass

    cmd.barrier(rt_color, vk::ImageUsage::ColorAttachment);
    cmd.begin_renderpass(render_target, {vk::LoadOp::clear_color(), vk::LoadOp::clear_depth()});
    cmd.bind_index_buffer(model_index_buffer, VK_INDEX_TYPE_UINT32, 0);
    cmd.bind_storage_buffer(graphics_program, model_vertex_buffer, 0);

    for (const auto& node : model.nodes)
    {
        if (node.mesh == (uint32_t)-1)
            continue;

        uniform_offset = global_uniform_buffer.push(&node.transform, sizeof(bul::mat4f));

        const auto& mesh = model.meshes[node.mesh];
        for (const auto& primitive : mesh.primitives)
        {
            const auto& material = model.materials[primitive.material];
            const auto& image_handle = model_images[model.textures[material.base_color_tex].source_image];

            cmd.bind_uniform_buffer(graphics_program, global_uniform_buffer.buffer_handle, 1, uniform_offset,
                                    sizeof(bul::mat4f));
            cmd.bind_image(graphics_program, image_handle, 2);
            cmd.bind_pipeline(graphics_program);

            cmd.draw_indexed(primitive.index_count, primitive.index_start);
        }
    }
    cmd.end_renderpass();

    // tonemap

    cmd.barrier(rt_color, vk::ImageUsage::ComputeShaderRead);
    cmd.barrier(fc.image, vk::ImageUsage::ComputeShaderReadWrite);
    cmd.bind_image(tonemap_program, rt_color, 0);
    cmd.bind_image(tonemap_program, fc.image, 1);
    cmd.bind_pipeline(tonemap_program);
    cmd.dispatch(bul::window::size().x, bul::window::size().y);

    // gui

    /* cmd.barrier(fc.image, vk::ImageUsage::ColorAttachment);
    cmd.begin_renderpass(fc.framebuffer, {vk::LoadOp::load()});
    vk::imgui_new_frame();
    render_gui();
    vk::imgui_render();
    vk::imgui_render_draw_data(cmd);
    cmd.end_renderpass(); */

    // present

    cmd.barrier(fc.image, vk::ImageUsage::Present);
    p_device->submit(cmd, fc.image_acquired_semaphore, fc.rendering_finished_semaphore, fc.rendering_finished_fence);

    if (!p_device->present(*p_surface))
    {
        resize();
    }

    ++frame_number;
}

void Renderer::render_gui()
{
    ImGui::Begin("ImGui");
    ImGui::Text("FPS: %u", uint32_t(1.0 / bul::time::delta_s()));

    if (ImGui::CollapsingHeader("Camera"))
    {
        auto position = camera.get_pos();
        if (ImGui::InputFloat3("Position", (float*)&position))
        {
            camera.set_position(position);
        }

        ImGui::Separator();
        const auto& front = camera.get_front();
        const auto& right = camera.get_right();
        const auto& up = camera.get_up();
        ImGui::Text("Front | %7.2f | %7.2f | %7.2f |", front.x, front.y, front.z);
        ImGui::Text("Right | %7.2f | %7.2f | %7.2f |", right.x, right.y, right.z);
        ImGui::Text("Up    | %7.2f | %7.2f | %7.2f |", up.x, up.y, up.z);

        ImGui::Separator();
        float speed = camera.get_speed();
        if (ImGui::DragFloat("Speed", &speed, 1, 0, 500))
        {
            camera.set_speed(speed);
        }

        ImGui::Separator();
        static int proj_radio_button = 0;
        float fov = camera.get_fov();
        if (ImGui::RadioButton("Perspective", &proj_radio_button, 0))
        {
            camera.set_perspective(fov);
        }
        if (ImGui::DragFloat("FOV", &fov, 1, 1, 179, "%.0f", 1))
        {
            camera.set_perspective(fov);
        }

        float ortho_size = camera.get_ortho_size();
        if (ImGui::RadioButton("Orthographic", &proj_radio_button, 1))
        {
            camera.set_orthographic(ortho_size);
        }
        if (ImGui::DragFloat("Size", &ortho_size, 1, 1, 360, "%.0f", 1))
        {
            camera.set_orthographic(ortho_size);
        }
    }

    ImGui::End();
}
