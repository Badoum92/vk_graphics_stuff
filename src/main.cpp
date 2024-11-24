#include "vk/context.h"
#include "vk/surface.h"

#include "test_renderer.h"
// #include "renderer.h"
// #include "path_tracing_renderer.h"
#include "camera.h"
#include "image.h"

#include "bul/math/math.h"
#include "bul/time.h"
#include "bul/window.h"
#include "bul/input.h"
#include "bul/log.h"

#include "imgui/imgui_impl_vulkan.h"
#include "imgui/imgui_impl_win32.h"

int main(int, char**)
{
    image image = image::from_file("resources/undefined.png");

    bul::window main_window;
    bul::window::create(&main_window, "window", {1280, 720});
    vk::context vk_context = vk::context::create(&main_window, true);

    ImGui::CreateContext();
    ImGui_ImplVulkan_InitInfo imgui_vulkan = {};
    imgui_vulkan.Instance = vk_context.instance;
    imgui_vulkan.PhysicalDevice = vk_context.physical_device;
    imgui_vulkan.Device = vk_context.device;
    imgui_vulkan.Queue = vk_context.graphics_queue;
    imgui_vulkan.DescriptorPool = vk_context.descriptor_pool;
    imgui_vulkan.MinImageCount = vk_context.surface.images.size;
    imgui_vulkan.ImageCount = vk_context.surface.images.size;
    imgui_vulkan.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    imgui_vulkan.UseDynamicRendering = true;
    imgui_vulkan.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    imgui_vulkan.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    imgui_vulkan.PipelineRenderingCreateInfo.pColorAttachmentFormats =
        &vk_context.images.get(vk_context.surface.images[0]).description.format;
    ImGui_ImplVulkan_Init(&imgui_vulkan);
    ImGui_ImplWin32_Init(main_window.handle);
    ImGui_ImplVulkan_CreateFontsTexture();
    ImGui::GetIO().DisplaySize.x = (float)main_window.size.x;
    ImGui::GetIO().DisplaySize.y = (float)main_window.size.y;

    vk::buffer_description buffer_description = {};
    buffer_description.size = image.size_bytes();
    buffer_description.usage = vk::transfer_buffer_usage;
    buffer_description.memory_usage = VMA_MEMORY_USAGE_AUTO;
    buffer_description.name = "staging buffer";
    bul::handle<vk::buffer> staging_buffer_handle = vk_context.create_buffer(buffer_description);

    vk::image_description image_description = {};
    image_description.width = image.width;
    image_description.height = image.height;
    image_description.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_description.name = "undefined image";
    vk_context.undefined_image_handle = vk_context.create_image(image_description);

    vk::command_buffer* command_buffer = vk_context.transfer_commands.get_command_buffer();
    command_buffer->upload_image(vk_context.undefined_image_handle, staging_buffer_handle, image.bytes,
                                 image.size_bytes());
    command_buffer->barrier(vk_context.undefined_image_handle, vk::image_usage::graphics_shader_read);
    vk_context.submit(command_buffer);
    vk_context.wait_idle();
    vk_context.destroy_buffer(staging_buffer_handle);
    image.destroy();

    vk_context.undefined_descriptor = vk_context.descriptor_set.create_descriptor(
        &vk_context, vk_context.undefined_image_handle, vk_context.default_sampler,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    ASSERT(vk_context.undefined_descriptor == 0);

    test_renderer test_renderer = test_renderer::create(&vk_context);

    camera camera;
    camera.position = {0.0f, 0.0f, 1.0f};
    camera.yaw = 0;
    camera.pitch = 0;
    camera.roll = 0;
    camera.fov_y = bul_radians(60.0f);
    camera.aspect_ratio = main_window.aspect_ratio();
    camera.near_plane = 1.0f;
    camera.far_plane = 10000.0f;
    camera.compute_view_proj();

    while (!main_window.should_close)
    {
        bul::time_update();

        bul::window::poll_events();

        if (bul::key_pressed(bul::key::l_alt))
        {
            main_window.show_cursor(!main_window.is_cursor_visible);
        }

        if (bul::key_pressed(bul::key::escape))
        {
            break;
        }

        constexpr float speed = 500.0f;
        if (bul::key_down(bul::key::Q))
        {
            camera.position -= camera.right * speed * bul::frame_delta_s;
        }
        if (bul::key_down(bul::key::D))
        {
            camera.position += camera.right * speed * bul::frame_delta_s;
        }
        if (bul::key_down(bul::key::Z))
        {
            bul::vec3f direction = {camera.forward.x, 0, camera.forward.z};
            direction = bul::normalize(direction);
            camera.position += direction * speed * bul::frame_delta_s;
        }
        if (bul::key_down(bul::key::S))
        {
            bul::vec3f direction = {camera.forward.x, 0, camera.forward.z};
            direction = bul::normalize(direction);
            camera.position -= direction * speed * bul::frame_delta_s;
        }
        if (bul::key_down(bul::key::space))
        {
            camera.position += camera::WORLD_UP * speed * bul::frame_delta_s;
        }
        if (bul::key_down(bul::key::C))
        {
            camera.position -= camera::WORLD_UP * speed * bul::frame_delta_s;
        }

        if (!main_window.is_cursor_visible)
        {
            bul::vec3f camera_rotation;
            camera_rotation.x = bul::mouse_position_delta.y * bul::frame_delta_s * 10.0f;
            camera_rotation.y = bul::mouse_position_delta.x * bul::frame_delta_s * 10.0f;
            camera_rotation.z = 0.0f;
            camera.rotate(camera_rotation);
            camera.compute_view_proj();
        }

        vk::frame_context* frame_context = vk_context.acquire_next_image();
        if (!frame_context)
        {
            test_renderer.resize();
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        test_renderer.draw(frame_context, &camera);

        ImGui::EndFrame();

        if (!vk_context.present(frame_context))
        {
            test_renderer.resize();
        }
    }

    vk_context.wait_idle();

    test_renderer.destroy();

    ImGui_ImplWin32_Shutdown();
    ImGui_ImplVulkan_Shutdown();
    ImGui::DestroyContext();

    vk_context.destroy();
    main_window.destroy();
}
