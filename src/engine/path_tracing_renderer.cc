#include "path_tracing_renderer.hh"

#include <iostream>
#include <bul/math/matrix.h>
#include <bul/window.h>
#include <bul/time.h>

#include "context.hh"
#include "device.hh"
#include "surface.hh"
#include "imgui.hh"

#include "vox_loader.hh"

// add some sky color / intensity
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

struct VoxelMaterial
{
    bul::vec3f base_color = bul::vec3f(1.0f, 0.0f, 1.0f);
    float metalness = 0.0f;
    bul::vec3f emissive = bul::vec3f(1.0f, 0.0f, 1.0f);
    float roughness = 0.0f;
    float ior = 1.0f;
    float transparency = 0.0f;
    bul::vec2f padding;
};

PathTracingRenderer PathTracingRenderer::create(vk::Context& context, vk::Device& device, vk::Surface& surface)
{
    PathTracingRenderer renderer;
    renderer.p_context = &context;
    renderer.p_device = &device;
    renderer.p_surface = &surface;
    return renderer;
}

void PathTracingRenderer::destroy()
{
    // vk::imgui_shutdown();
}

void PathTracingRenderer::init()
{
    resize();
    reload_shaders();

    // vk::imgui_init(*p_context, *p_device, *p_surface);

    camera.set_speed(50.0f);

    auto& cmd = p_device->get_graphics_command();
    {
        Vox::Model model;
        // model.load("../models/voxel-model/vox/scan/dragon.vox");
        // model.load("../models/voxel-model/vox/scan/teapot.vox");
        // model.load("../models/voxel-model/vox/monument/monu7.vox");
        // model.load("../models/voxel-model/vox/monument/monu5.vox");
        model.load("../models/materials.vox");
        // model.load("../models/testscene.vox");
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
            auto& material = voxel_materials_data[i];
            const auto& matl = model.materials[i];
            const auto& color = model.palette[i];
            material.base_color.x = color.r / 255.0f;
            material.base_color.y = color.g / 255.0f;
            material.base_color.z = color.b / 255.0f;
            material.emissive = material.base_color * matl.emit * std::powf(2, matl.flux);
            material.metalness = matl.metal;
            material.roughness = matl.rough;
            material.ior = matl.ior + 1;
            material.transparency = matl.trans;
        }

        voxel_materials = p_device->create_buffer(
            {.size = static_cast<uint32_t>(voxel_materials_data.size() * sizeof(VoxelMaterial))});
        cmd.upload_buffer(voxel_materials, voxel_materials_data.data(),
                          static_cast<uint32_t>(voxel_materials_data.size() * sizeof(VoxelMaterial)));
    }
    {
        global_uniform_buffer = vk::RingBuffer::create(
            *p_device,
            {.size = 4 * MB, .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, .memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU});
    }
    p_device->submit_blocking(cmd);
}

void PathTracingRenderer::resize()
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

    {
        p_device->destroy_image(color);
        p_device->destroy_image(color_acc);

        color = p_device->create_image({.width = bul::window::size().x,
                                        .height = bul::window::size().y,
                                        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                                        .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT});

        color_acc = p_device->create_image({.width = bul::window::size().x,
                                            .height = bul::window::size().y,
                                            .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                                            .usage = VK_IMAGE_USAGE_STORAGE_BIT});
    }
}

void PathTracingRenderer::reload_shaders()
{
    p_device->wait_idle();
    shaders_need_reload = false;

    {
        vk::ComputeProgramDescription prog_desc{};
        prog_desc.descriptor_types = {vk::DescriptorType::create(vk::DescriptorType::Type::StorageImage),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::StorageImage),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::StorageImage),
                                      vk::DescriptorType::create(vk::DescriptorType::Type::StorageBuffer)};
        prog_desc.shader = p_device->create_shader("shaders/voxel_raytrace.comp");
        raytracing_program = p_device->create_compute_program(prog_desc);
    }
}

void PathTracingRenderer::render()
{
    if (!p_device->acquire_next_image(*p_surface))
    {
        resize();
        return;
    }

    if (shaders_need_reload)
    {
        reload_shaders();
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

    GlobalUniformSet global_uniform_set = {};
    global_uniform_set.view = camera.get_view();
    global_uniform_set.inv_view = camera.get_inv_view();
    global_uniform_set.proj = camera.get_proj();
    global_uniform_set.inv_proj = camera.get_inv_proj();
    global_uniform_set.camera_pos = bul::vec4f(camera.get_pos(), 1.0f);
    global_uniform_set.resolution = {bul::window::size().x, bul::window::size().y};
    global_uniform_set.exposure = exposure;
    global_uniform_set.frame_number = frame_number;
    uint32_t uniform_offset = global_uniform_buffer.push(&global_uniform_set, sizeof(GlobalUniformSet));
    p_device->global_uniform_set.bind_uniform_buffer(0, global_uniform_buffer.buffer_handle, uniform_offset,
                                                     sizeof(GlobalUniformSet));

    // main renderpass

    cmd.barrier(color, vk::ImageUsage::ComputeShaderReadWrite);
    cmd.barrier(color_acc, vk::ImageUsage::ComputeShaderReadWrite);
    cmd.bind_descriptor_set(raytracing_program, p_device->global_uniform_set, 0);
    cmd.bind_image(raytracing_program, color, 0);
    cmd.bind_image(raytracing_program, color_acc, 1);
    cmd.bind_image(raytracing_program, voxels, 2);
    cmd.bind_storage_buffer(raytracing_program, voxel_materials, 3);
    cmd.bind_pipeline(raytracing_program);
    cmd.dispatch(bul::window::size().x, bul::window::size().y);

    cmd.barrier(color, vk::ImageUsage::TransferSrc);
    cmd.barrier(fc.image, vk::ImageUsage::TransferDst);
    cmd.blit_image(color, fc.image);

    // gui

    cmd.barrier(fc.image, vk::ImageUsage::ColorAttachment);
    cmd.begin_renderpass(fc.framebuffer, {vk::LoadOp::load()});
    // vk::imgui_new_frame();
    // render_gui();
    // vk::imgui_render();
    // vk::imgui_render_draw_data(cmd);
    cmd.end_renderpass();

    // present

    cmd.barrier(fc.image, vk::ImageUsage::Present);
    p_device->submit(cmd, fc.image_acquired_semaphore, fc.rendering_finished_semaphore, fc.rendering_finished_fence);

    if (!p_device->present(*p_surface) && bul::window::resized())
    {
        resize();
    }

    ++frame_number;
}

void PathTracingRenderer::render_gui()
{
    ImGui::Begin("ImGui");
    ImGui::Text("FPS: %u", uint32_t(1.0 / bul::time::delta_s()));

    if (ImGui::CollapsingHeader("Rendering"))
    {
        if (ImGui::Button("Reload shaders"))
        {
            shaders_need_reload = true;
        }
        ImGui::SliderFloat("Exposure", &exposure, 0, 10);
    }

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
        if (ImGui::SliderFloat("Speed", &speed, 0, 500))
        {
            camera.set_speed(speed);
        }

        ImGui::Separator();
        static int proj_radio_button = 0;
        float fov = camera.get_fov();
        if (ImGui::RadioButton("Perspective", &proj_radio_button, 0)
            || (proj_radio_button == 0 && ImGui::SliderFloat("FOV", &fov, 0, 180, "%.0f")))
        {
            camera.set_perspective(fov);
            frame_number = 0;
        }

        float ortho_size = camera.get_ortho_size();
        if (ImGui::RadioButton("Orthographic", &proj_radio_button, 1)
            || (proj_radio_button == 1 && ImGui::SliderFloat("Size", &ortho_size, 0, 500, "%.0f")))
        {
            camera.set_orthographic(ortho_size);
            frame_number = 0;
        }
    }

    ImGui::End();
}
