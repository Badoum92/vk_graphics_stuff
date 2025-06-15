#include "vk/vk_context.h"
#include "vk/vk_surface.h"

#include "test_renderer.h"
#include "test_compute.h"
// #include "renderer.h"
// #include "path_tracing_renderer.h"
#include "camera.h"
#include "image.h"

#include "core/math/math.h"
#include "core/time.h"
#include "core/window.h"
#include "core/input.h"
#include "core/log.h"
#include "core/memory/linear_allocator.h"
#include "core/thread.h"

#include "imgui.h"

#include "tracy/Tracy.hpp"

int main(int, char**)
{
    thread_init();

    char module_directory[256];
    GetModuleFileNameA(nullptr, module_directory, sizeof(module_directory));
    char* last_separator = strrchr(module_directory, '\\');
    if (last_separator)
        *last_separator = 0;
    SetCurrentDirectoryA(module_directory);

    linear_allocator linear_allocator = linear_allocator_create(MB(4));
    linear_allocator_set_global(&linear_allocator);

    window main_window;
    window_create(&main_window, "window", {1920, 1080});
    vk::context vk_context = vk::context::create(&main_window, true);

    imgui_init(&vk_context, &main_window);

    {
        image image = image::from_file("resources/undefined.png");
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

        vk_context.undefined_descriptor = vk_context.texture_descriptor_set.create_texture_descriptor(
            &vk_context, vk_context.undefined_image_handle, vk_context.default_sampler);
        ASSERT(vk_context.undefined_descriptor == 0);
    }

#if 1
    test_renderer test_renderer =
        test_renderer::create(&vk_context, vk_context.surface.extent.width, vk_context.surface.extent.height);
#else
    test_compute test_renderer =
        test_compute::create(&vk_context, vk_context.surface.extent.width, vk_context.surface.extent.height);
#endif

    camera camera = camera::create();
    camera.position = {0.0f, 1.0f, 1.0f};
    camera.yaw = 0;
    camera.pitch = 0;
    camera.roll = 0;
    camera.fov_y = math_radians(70.0f);
    camera.aspect_ratio = window_aspect_ratio(&main_window);
    camera.near_plane = 1.0f;
    camera.far_plane = 10000.0f;
    camera.compute_view_proj();
    float speed = 50;

    imgui_begin_frame();

    imgui_docknode main_docknode = imgui_docknode::begin_new(imgui_global_dockspace);
    auto [up, log_docknode] = main_docknode.split_v(0.8f);
    auto [left, viewport_docknode] = up.split_h(0.1f);
    auto [debug_docknode, stats_docknode] = left.split_v(0.5f);
    viewport_docknode.dock_window("Viewport");
    log_docknode.dock_window("Logs");
    debug_docknode.dock_window("Debug");
    stats_docknode.dock_window("Stats");
    main_docknode.end();

    ImGuiWindowClass viewport_window_class = {};
    viewport_window_class.ClassId = ImGui::GetID("Viewport");
    viewport_window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_AutoHideTabBar;

    float avg_frame_ms = 0.0f;

    while (!main_window.should_close)
    {
        ZoneScoped;

        linear_reset_globals();

        time_update();

        window_poll_events();

        if (is_key_pressed(KEY_ESCAPE))
        {
            break;
        }

        if (is_key_pressed(KEY_L_ALT))
        {
            window_show_cursor(&main_window, !main_window.is_cursor_visible);
        }

        vec3f direction = {0, 0, 0};
        if (is_key_down(KEY_Q))
        {
            direction -= camera.right;
        }
        if (is_key_down(KEY_D))
        {
            direction += camera.right;
        }
        if (is_key_down(KEY_Z))
        {
            direction += {camera.forward.x, 0, camera.forward.z};
        }
        if (is_key_down(KEY_S))
        {
            direction -= {camera.forward.x, 0, camera.forward.z};
        }
        if (is_key_down(KEY_SPACE))
        {
            direction += camera::WORLD_UP;
        }
        if (is_key_down(KEY_C))
        {
            direction -= camera::WORLD_UP;
        }
        if (direction != vec3f{0, 0, 0})
        {
            direction = vec_normalize(direction);
            camera.position += direction * speed * time_get_delta_s();
        }

        if (!main_window.is_cursor_visible)
        {
            vec2i mouse_delta = mouse_get_delta();
            vec3f camera_rotation;
            camera_rotation.x = mouse_delta.y * time_get_delta_s() * 10.0f;
            camera_rotation.y = mouse_delta.x * time_get_delta_s() * 10.0f;
            camera_rotation.z = 0.0f;
            camera.rotate(camera_rotation);
        }

        camera.compute_view_proj();

        vk::frame_context* frame_context;
        frame_context = vk_context.acquire_next_image();
        ASSERT(frame_context != nullptr);

        // Actual rendering
        test_renderer.draw(frame_context, &camera);

        // ImGui stuff
        frame_context->command_buffer->barrier(frame_context->image, vk::image_usage::color_attachment);
        frame_context->command_buffer->begin_rendering({{frame_context->image}}, {{vk::load_op::clear_color()}},
                                                       bul::handle<vk::image>::invalid(), vk::load_op::dont_care());

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowClass(&viewport_window_class);
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImVec2 window_size = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
        uint32_t window_width = (uint32_t)window_size.x;
        uint32_t window_height = (uint32_t)window_size.y;
        vk::image& render_target_image = vk_context.images.get(test_renderer.render_target.image);
        ImGui::Image(
            (ImTextureID)test_renderer.render_target.vk_descriptorset,
            ImVec2((float)render_target_image.description.width, (float)render_target_image.description.height));
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::Begin("Stats");
        avg_frame_ms = avg_frame_ms * 0.9f + time_to_ms(time_get_delta()) * 0.1f;
        ImGui::Text("FPS:  %u", (uint32_t)(1000.0f / avg_frame_ms));
        ImGui::Text("Time: %g ms", avg_frame_ms);
        ImGui::End();

        ImGui::Begin("Debug");
        ImGui::InputFloat("Speed", &speed);
        if (ImGui::TreeNode("Input"))
        {
            vec2i mouse_position = mouse_get_position();
            ImGui::Text("Mouse");
            ImGui::Text("%d %d", mouse_position.x, mouse_position.y);
            ImGui::TreePop();
        }

        ImGui::End();

        imgui_end_frame(frame_context->command_buffer);
        frame_context->command_buffer->end_rendering();

        // End frame
        frame_context->command_buffer->barrier(frame_context->image, vk::image_usage::present);
        vk_context.submit(frame_context->command_buffer, frame_context);

        // ImGui::UpdatePlatformWindows();
        // ImGui::RenderPlatformWindowsDefault();

        vk_context.present(frame_context);

        if (window_width != test_renderer.width || window_height != test_renderer.height)
        {
            camera.aspect_ratio = window_size.x / window_size.y;
            camera.compute_view_proj();
            test_renderer.resize(window_width, window_height);
        }

        imgui_begin_frame();

        FrameMark;
    }

    vk_context.wait_idle();

    test_renderer.destroy();

    imgui_shutdown();

    vk_context.destroy();
    window_destroy(&main_window);

    linear_allocator_destroy(&linear_allocator);
}
