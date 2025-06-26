#include "vk/vk_context.h"
#include "vk/vk_surface.h"
#include "vk/vk_image.h"
#include "vk/vk_buffer.h"
#include "vk/vk_shader.h"

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
#include "core/thread_pool.h"
#include "core/directory_watcher.h"

#include "imgui.h"
#include "globals.h"

#include "tracy/Tracy.hpp"

static int64_t recompile_shaders = 0;

void shader_changed(const char* file, FILE_NOTIFY)
{
    if (!strstr(file, "spv"))
        recompile_shaders = time_now();
}

void set_working_directory()
{
    char module_directory[MAX_PATH];
    GetModuleFileNameA(nullptr, module_directory, sizeof(module_directory));
    char* last_separator = strrchr(module_directory, '\\');
    if (last_separator)
        *last_separator = 0;
    SetCurrentDirectoryA(module_directory);
}

int main(int, char**)
{
    set_working_directory();

    thread_init();
    thread_pool_init();

    linear_allocator linear_allocator = linear_allocator_create(MB(4));
    linear_allocator_set_global(&linear_allocator);

    directory_watcher watcher;
    directory_watcher_run(&watcher, "shaders", shader_changed);

    window main_window;
    window_create(&main_window, "window", {1920, 1080});
    vk::context vk_context;
    vk::context::create(&vk_context, &main_window, true);

    imgui_init(&vk_context, &main_window);

    {
        image image = image::from_file("resources/undefined.png");
        vk::buffer_description buffer_description = {};
        buffer_description.size = image.size_bytes();
        buffer_description.usage = vk::transfer_buffer_usage;
        buffer_description.memory_usage = VMA_MEMORY_USAGE_AUTO;
        buffer_description.name = "staging buffer";
        vk::buffer* staging_buffer = vk_context.create_buffer(buffer_description);

        vk::image_description image_description = {};
        image_description.width = image.width;
        image_description.height = image.height;
        image_description.format = VK_FORMAT_R8G8B8A8_UNORM;
        image_description.name = "undefined image";
        vk_context.undefined_image = vk_context.create_image(image_description);

        vk::command_buffer* command_buffer = vk_context.transfer_commands.get_command_buffer();
        command_buffer->upload_image(vk_context.undefined_image, staging_buffer, image.bytes, image.size_bytes());
        command_buffer->barrier(vk_context.undefined_image, vk::image_usage::graphics_shader_read);
        vk_context.submit(command_buffer);
        vk_context.wait_idle();
        vk_context.destroy_buffer(staging_buffer);
        image.destroy();

        vk_context.undefined_descriptor = vk_context.texture_descriptor_set.create_texture_descriptor(
            &vk_context, vk_context.undefined_image, vk_context.default_sampler);
        ASSERT(vk_context.undefined_descriptor == 0);
    }

#if 0
    test_renderer test_renderer =
        test_renderer::create(&vk_context, vk_context.surface.extent.width, vk_context.surface.extent.height);
#else
    test_compute test_renderer =
        test_compute::create(&vk_context, vk_context.surface.extent.width, vk_context.surface.extent.height);
#endif

    camera camera = camera_create();
    camera.position = {0.0f, 1.0f, 1.0f};
    camera.yaw = 0;
    camera.pitch = 0;
    camera.roll = 0;
    camera.fov_y = math_radians(70.0f);
    camera.aspect_ratio = window_aspect_ratio(&main_window);
    camera.near_plane = 1.0f;
    camera.far_plane = 10000.0f;
    camera_compute_view_proj(&camera);
    float speed = 200.0f;
    float sensitivity = 0.5f;
    bool change_vsync = false;

    imgui_begin_frame();

    ImGuiID main_docknode = imgui_docknode_main();
    imgui_docknode_begin(main_docknode);
    auto [up, log_docknode] = imgui_docknode_split_h(main_docknode, 0.8f);
    auto [left, viewport_docknode] = imgui_docknode_split_v(up, 0.1f);
    auto [debug_docknode, stats_docknode] = imgui_docknode_split_h(left, 0.5f);
    imgui_docknode_window(viewport_docknode, "Viewport");
    imgui_docknode_window(log_docknode, "Logs");
    imgui_docknode_window(debug_docknode, "Debug");
    imgui_docknode_window(stats_docknode, "Stats");
    imgui_docknode_end(main_docknode);

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
            input_show_cursor(!input_is_cursor_visible());
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
            direction += WORLD_UP;
        }
        if (is_key_down(KEY_C))
        {
            direction -= WORLD_UP;
        }
        if (direction != vec3f{0, 0, 0})
        {
            direction = vec_normalize(direction);
            camera.position += direction * speed * time_get_delta_s();
        }

        if (!input_is_cursor_visible())
        {
            vec2i mouse_delta = input_get_mouse_delta();
            vec3f rotation;
            rotation.x = mouse_delta.y * sensitivity * 0.01f;
            rotation.y = mouse_delta.x * sensitivity * 0.01f;
            rotation.z = 0.0f;
            camera_rotate(&camera, rotation);
        }

        camera_compute_view_proj(&camera);

        vk::frame_context* frame_context;
        frame_context = vk_context.acquire_next_image();
        ASSERT(frame_context != nullptr);

        // Actual rendering
        test_renderer.draw(frame_context, &camera);

        // ImGui stuff
        frame_context->command_buffer->barrier(frame_context->image, vk::image_usage::color_attachment);
        frame_context->command_buffer->begin_rendering({{frame_context->image}}, {{vk::load_op::clear_color()}},
                                                       nullptr, vk::load_op::dont_care());

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::SetNextWindowClass(&viewport_window_class);
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar);
        ImVec2 window_size = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
        uint32_t window_width = (uint32_t)window_size.x;
        uint32_t window_height = (uint32_t)window_size.y;
        ImGui::Image((ImTextureID)test_renderer.render_target.vk_descriptorset,
                     ImVec2((float)test_renderer.render_target.image->description.width,
                            (float)test_renderer.render_target.image->description.height));
        ImGui::End();
        ImGui::PopStyleVar();

        avg_frame_ms = avg_frame_ms * 0.9f + time_to_ms(time_get_delta()) * 0.1f;
        if (ImGui::Begin("Stats"))
        {
            ImGui::Text("FPS:  %u", (uint32_t)(1000.0f / avg_frame_ms));
            ImGui::Text("Time: %g ms", avg_frame_ms);
        }
        ImGui::End();

        if (ImGui::Begin("Debug"))
        {
            if (ImGui::Button("Reload shaders")
                || ImGui::IsKeyChordPressed(ImGuiKey_ModCtrl | ImGuiKey_ModShift | ImGuiKey_R))
            {
                recompile_shaders = time_now();
            }
            change_vsync = ImGui::Checkbox("Vsync", &vk_context.vsync);
            ImGui::DragFloat("Speed", &speed, 1.0f, 0.0f, 2000.0f, "%g");
            if (ImGui::CollapsingHeader("Input", ImGuiTreeNodeFlags_DefaultOpen))
            {
                vec2i mouse_position = input_get_mouse_position();
                vec2i mouse_delta = input_get_mouse_delta();
                ImGui::Text("Mouse");
                ImGui::DragFloat("Sensitivity", &sensitivity, 0.01f, 0.0f, 100.0f, "%g");
                ImGui::Text("position: %d %d", mouse_position.x, mouse_position.y);
                ImGui::Text("delta: %d %d", mouse_delta.x, mouse_delta.y);
            }
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
            camera_compute_view_proj(&camera);
            test_renderer.resize(window_width, window_height);
        }

        if (change_vsync)
        {
            change_vsync = false;
            vk_context.wait_idle();
            vk_context.set_vsync(vk_context.vsync);
        }

        if (recompile_shaders != 0 && time_to_ms(time_now() - recompile_shaders) > 100)
        {
            recompile_shaders = 0;
            if (vk_compile_shaders())
            {
                test_renderer.reload_shaders();
            }
        }

        imgui_begin_frame();

        g_frame++;

        FrameMark;
    }

    vk_context.wait_idle();

    test_renderer.destroy();

    imgui_shutdown();

    vk_context.destroy();
    window_destroy(&main_window);

    directory_watcher_stop(&watcher);

    linear_allocator_destroy(&linear_allocator);

    thread_pool_shutdown();
}
