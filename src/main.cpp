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

#include "imgui.h"

#include "tracy/Tracy.hpp"

int main(int, char**)
{
    image image = image::from_file("resources/undefined.png");

    bul::window main_window;
    bul::window::create(&main_window, "window", {1280, 720});
    vk::context vk_context = vk::context::create(&main_window, true);

    imgui_init(&vk_context, &main_window);

    {
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
    }

    test_renderer test_renderer =
        test_renderer::create(&vk_context, vk_context.surface.extent.width, vk_context.surface.extent.height);

    camera camera;
    camera.position = {0.0f, 0.0f, 1.0f};
    camera.yaw = 0;
    camera.pitch = 0;
    camera.roll = 0;
    camera.fov_y = bul_radians(90.0f);
    camera.aspect_ratio = main_window.aspect_ratio();
    camera.near_plane = 1.0f;
    camera.far_plane = 10000.0f;
    camera.compute_view_proj();

    while (!main_window.should_close)
    {
        ZoneScoped;

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

        static constexpr float speed = 500;
        bul::vec3f direction = {0, 0, 0};
        if (bul::key_down(bul::key::Q))
        {
            direction -= camera.right;
        }
        if (bul::key_down(bul::key::D))
        {
            direction += camera.right;
        }
        if (bul::key_down(bul::key::Z))
        {
            direction += {camera.forward.x, 0, camera.forward.z};
        }
        if (bul::key_down(bul::key::S))
        {
            direction -= {camera.forward.x, 0, camera.forward.z};
        }
        if (bul::key_down(bul::key::space))
        {
            direction += camera::WORLD_UP;
        }
        if (bul::key_down(bul::key::C))
        {
            direction -= camera::WORLD_UP;
        }
        if (direction != bul::vec3f{0, 0, 0})
        {
            direction = bul::normalize(direction);
            camera.position += direction * speed * bul::frame_delta_s;
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

        vk::frame_context* frame_context;
        frame_context = vk_context.acquire_next_image();
        if (!frame_context)
        {
            test_renderer.resize(vk_context.surface.extent.width, vk_context.surface.extent.height);
        }

        imgui_begin_frame();

        test_renderer.draw(frame_context, &camera);

        frame_context->command_buffer->begin_rendering({{frame_context->image}}, {{vk::load_op::load()}},
                                                       bul::handle<vk::image>::invalid(), vk::load_op::dont_care());
        ImGui::Begin("Stats");
        ImGui::Text("frame time: %g ms", bul::ticks_to_ms_f(bul::avg_frame_delta_ticks));
        ImGui::Text("FPS: %g", 1.0f / bul::ticks_to_s_f(bul::avg_frame_delta_ticks));
        ImGui::Text("%d %d", bul::mouse_position.x, bul::mouse_position.y);
        ImGui::Text("%g %g", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
        ImGui::End();

        imgui_end_frame(frame_context->command_buffer);
        frame_context->command_buffer->end_rendering();

        frame_context->command_buffer->barrier(frame_context->image, vk::image_usage::present);
        vk_context.submit(frame_context->command_buffer, frame_context);

        // ImGui::UpdatePlatformWindows();
        // ImGui::RenderPlatformWindowsDefault();

        if (!vk_context.present(frame_context))
        {
            test_renderer.resize(vk_context.surface.extent.width, vk_context.surface.extent.height);
        }

        FrameMark;
    }

    vk_context.wait_idle();

    test_renderer.destroy();

    imgui_shutdown();

    vk_context.destroy();
    main_window.destroy();
}
