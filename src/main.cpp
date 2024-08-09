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

int main(int, char**)
{
    image image = image::from_file("resources/undefined.png");

    bul::window main_window;
    bul::window::create(&main_window, "window", {1280, 720});
    vk::context vk_context = vk::context::create(&main_window, true);

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

    int64_t previous_tick = bul::current_tick();
    int64_t current_tick = previous_tick;

    while (!main_window.should_close)
    {
        current_tick = bul::current_tick();
        float delta_time = bul::ticks_to_s_f(current_tick - previous_tick);
        previous_tick = current_tick;

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
            camera.move_right(-speed * delta_time);
        }
        if (bul::key_down(bul::key::D))
        {
            camera.move_right(speed * delta_time);
        }
        if (bul::key_down(bul::key::Z))
        {
            camera.move_forward(speed * delta_time);
        }
        if (bul::key_down(bul::key::S))
        {
            camera.move_forward(-speed * delta_time);
        }
        if (bul::key_down(bul::key::space))
        {
            camera.move_up(speed * delta_time);
        }
        if (bul::key_down(bul::key::C))
        {
            camera.move_up(-speed * delta_time);
        }

        if (!main_window.is_cursor_visible)
        {
            bul::vec3f camera_rotation;
            camera_rotation.x = bul::mouse_position_delta.y * delta_time * 10.0f;
            camera_rotation.y = bul::mouse_position_delta.x * delta_time * 10.0f;
            camera_rotation.z = 0.0f;
            camera.rotate(camera_rotation);
            camera.compute_view_proj();
        }

        vk::frame_context* frame_context = vk_context.acquire_next_image();
        if (!frame_context)
        {
            test_renderer.resize();
        }

        test_renderer.draw(frame_context, &camera, delta_time);

        if (!vk_context.present(frame_context))
        {
            test_renderer.resize();
        }
    }

    vk_context.wait_idle();

    test_renderer.destroy();

    vk_context.destroy();
    main_window.destroy();
}
