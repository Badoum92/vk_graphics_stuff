#include "renderer.hh"

#include <iostream>
#include <glm/glm.hpp>

#include "window.hh"
#include "input.hh"
#include "time.hh"
#include "context.hh"
#include "device.hh"
#include "surface.hh"
#include "imgui.hh"

#include "vox_loader.hh"

struct GlobalUniformSet
{
    glm::mat4 view;
    glm::mat4 inv_view;
    glm::mat4 proj;
    glm::mat4 inv_proj;
    glm::vec4 camera_pos;
    glm::uvec2 resolution;
    uint32_t frame_number;
    uint32_t pad0;
};

struct VoxelMaterial
{
    glm::vec4 albedo = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    glm::vec4 emissive = glm::vec4(0.0f);
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

    EventHandler::register_key_callback(this);
    EventHandler::register_cursor_pos_callback(this);

    camera.set_speed(50.0f);

    auto& cmd = p_device->get_graphics_command();
    {
        // model = gltf::load_model("../models/Sponza/glTF/Sponza.gltf", *p_device);
        // model = gltf::load_model("../models/backpack/scene.gltf", *p_device);
        for (auto& img : model.images)
        {
            cmd.barrier(img.handle, vk::ImageUsage::GraphicsShaderRead);
        }
    }
    {
        Vox::Model model("../models/vox/monu7.vox");
        const auto size = model.chunks[0].size;

        vk::ImageDescription image_desc{};
        image_desc.width = size->x;
        image_desc.height = size->y;
        image_desc.depth = size->z;
        image_desc.format = VK_FORMAT_R8_UINT;
        image_desc.type = VK_IMAGE_TYPE_3D;
        image_desc.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        voxels = p_device->create_image(image_desc);

        std::vector<uint8_t> voxels_data(size->x * size->y * size->z);
        for (size_t i = 0; i < model.chunks[0].n_voxels; ++i)
        {
            auto voxel = model.chunks[0].xyzi[i];
            voxels_data[voxel.z * size->x * size->y + voxel.y * size->x + voxel.x] = voxel.color_index;
        }

        cmd.upload_image(voxels, voxels_data.data(), voxels_data.size());
        cmd.barrier(voxels, vk::ImageUsage::ComputeShaderReadWrite);

        std::vector<VoxelMaterial> voxel_materials_data(model.palette.size());
        for (size_t i = 0; i < model.palette.size(); ++i)
        {
            voxel_materials_data[i].albedo.r = model.palette[i].r / 255.0f;
            voxel_materials_data[i].albedo.g = model.palette[i].g / 255.0f;
            voxel_materials_data[i].albedo.b = model.palette[i].b / 255.0f;
            voxel_materials_data[i].albedo.a = model.palette[i].a / 255.0f;
            if (voxel_materials_data[i].albedo.r > 0.9f || voxel_materials_data[i].albedo.b > 0.7f)
            {
                voxel_materials_data[i].emissive = voxel_materials_data[i].albedo;
            }
        }

        voxel_materials = p_device->create_buffer(
            {.size = static_cast<uint32_t>(voxel_materials_data.size() * sizeof(VoxelMaterial))});
        cmd.upload_buffer(voxel_materials, voxel_materials_data.data(),
                          static_cast<uint32_t>(voxel_materials_data.size() * sizeof(VoxelMaterial)));
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
        vk::ComputeProgramDescription prog_desc{};
        prog_desc.descriptor_types = {vk::DescriptorType::create(vk::DescriptorType::Type::StorageImage),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::StorageImage),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::StorageImage),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::StorageBuffer)};
        prog_desc.shader = p_device->create_shader("shaders/voxel_raytrace.comp");
        raytracing_program = p_device->create_compute_program(prog_desc);
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
    scissor.extent = {Window::width(), Window::height()};
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
        p_device->destroy_image(rt_color);
        p_device->destroy_image(rt_depth);
        p_device->destroy_image(color_acc);
        p_device->destroy_framebuffer(render_target);

        rt_color = p_device->create_image({.width = fb_desc.width,
                                           .height = fb_desc.height,
                                           .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                                           .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
                                               | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT});

        rt_depth = p_device->create_image({.width = fb_desc.width,
                                           .height = fb_desc.height,
                                           .format = VK_FORMAT_D32_SFLOAT,
                                           .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT});

        color_acc = p_device->create_image({.width = fb_desc.width,
                                            .height = fb_desc.height,
                                            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                                            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
                                                | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT});

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
    global_uniform_set.camera_pos = glm::vec4(camera.get_pos(), 1.0f);
    global_uniform_set.resolution = {Window::width(), Window::height()};
    global_uniform_set.frame_number = frame_number;
    uint32_t uniform_offset = global_uniform_buffer.push(&global_uniform_set, sizeof(GlobalUniformSet));
    p_device->global_uniform_set.bind_uniform_buffer(0, global_uniform_buffer.buffer_handle, uniform_offset,
                                                     sizeof(GlobalUniformSet));
    cmd.bind_descriptor_set(graphics_program, p_device->global_uniform_set, 0);

    // main renderpass

    /* cmd.barrier(rt_color, vk::ImageUsage::ColorAttachment);
    cmd.begin_renderpass(render_target, {vk::LoadOp::clear_color(), vk::LoadOp::clear_depth()});
    cmd.bind_index_buffer(model.index_buffer, VK_INDEX_TYPE_UINT32, 0);
    cmd.bind_storage_buffer(graphics_program, model.vertex_buffer, 0);

    for (const auto& node : model.nodes)
    {
        if (!node.mesh)
            continue;

        uniform_offset = global_uniform_buffer.push(&node.transform, sizeof(glm::mat4));

        const auto& mesh = model.meshes[*node.mesh];
        for (unsigned primitive_idx : mesh.primitives)
        {
            const auto& primitive = model.primitives[primitive_idx];
            const auto& material = model.materials[primitive.material];
            const auto& image_handle = model.images[model.textures[material.base_color_tex].source].handle;

            cmd.bind_uniform_buffer(graphics_program, global_uniform_buffer.buffer_handle, 1, uniform_offset,
                                    sizeof(glm::mat4));
            cmd.bind_image(graphics_program, image_handle, 2);
            cmd.bind_pipeline(graphics_program);

            cmd.draw_indexed(primitive.index_count, primitive.index_start);
        }
    }
    cmd.end_renderpass(); */

    cmd.barrier(rt_color, vk::ImageUsage::ComputeShaderReadWrite);
    cmd.barrier(color_acc, vk::ImageUsage::ComputeShaderReadWrite);
    cmd.bind_descriptor_set(raytracing_program, p_device->global_uniform_set, 0);
    cmd.bind_image(raytracing_program, rt_color, 0);
    cmd.bind_image(raytracing_program, color_acc, 1);
    cmd.bind_image(raytracing_program, voxels, 2);
    cmd.bind_storage_buffer(raytracing_program, voxel_materials, 3);
    cmd.bind_pipeline(raytracing_program);
    cmd.dispatch(Window::width(), Window::height());

    // tonemap

    cmd.barrier(rt_color, vk::ImageUsage::ComputeShaderRead);
    cmd.barrier(fc.image, vk::ImageUsage::ComputeShaderReadWrite);
    cmd.bind_image(tonemap_program, rt_color, 0);
    cmd.bind_image(tonemap_program, fc.image, 1);
    cmd.bind_pipeline(tonemap_program);
    cmd.dispatch(Window::width(), Window::height());

    // gui

    cmd.barrier(fc.image, vk::ImageUsage::ColorAttachment);
    cmd.begin_renderpass(fc.framebuffer, {vk::LoadOp::load()});
    vk::imgui_new_frame();
    render_imgui();
    vk::imgui_render();
    vk::imgui_render_draw_data(cmd);
    cmd.end_renderpass();

    // present

    cmd.barrier(fc.image, vk::ImageUsage::Present);
    p_device->submit(cmd, fc.image_acquired_semaphore, fc.rendering_finished_semaphore, fc.rendering_finished_fence);

    if (!p_device->present(*p_surface))
    {
        resize();
    }

    ++frame_number;
}

void Renderer::render_imgui()
{
    ImGui::Begin("ImGui");
    ImGui::Text("FPS: %u", Time::fps());

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
        renderer.frame_number = 0;
    }
}
