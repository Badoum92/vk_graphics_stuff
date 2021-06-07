#include "voxel_app.hh"

#include <iostream>
#include <glm/glm.hpp>

#include "vk_context/vk_context.hh"
#include "vk_imgui/vk_imgui.hh"
#include "input/input.hh"
#include "time/time.hh"
#include "window/window.hh"
#include "event/event.hh"

struct Vertex
{
    glm::vec3 pos;
    float _pad0;
    glm::vec2 tex_coord;
    glm::vec2 _pad1;
};

struct Voxel
{
    glm::vec4 pos;
    glm::vec4 color;
};

VoxelApp::VoxelApp()
{
    EventHandler::register_key_callback(this);
    EventHandler::register_cursor_pos_callback(this);

    // clang-format off
    const std::vector<Voxel> voxel_data = {
        {{0.0f, 0.0f, 2.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}
    };
    // clang-format on
    voxels.create_storage(voxel_data.data(), voxel_data.size() * sizeof(Voxel));
    global_uniform_buffer.create_uniform(4 * KB);

    PipelineInfo pipeline_info("shaders/voxel.vert.spv", "shaders/voxel.frag.spv");
    pipeline_info.set_topology(VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
    pipeline.create(pipeline_info);

    pipeline.descriptor_set(0).bind_buffer(0, global_uniform_buffer, sizeof(GlobalUniform));
    pipeline.descriptor_set(0).update();

    pipeline.descriptor_set(1).bind_buffer(0, voxels);
    pipeline.descriptor_set(1).update();
}

VoxelApp::~VoxelApp()
{}

void VoxelApp::update()
{
    camera.update();

    imgui_update();

    auto frame_data = VkContext::begin_frame();
    if (!frame_data.has_value())
        return;

    auto& [cmd, framebuffer] = frame_data.value();

    GlobalUniform global_uniform;
    global_uniform.view = camera.get_view();
    global_uniform.inv_view = camera.get_inv_view();
    global_uniform.proj = camera.get_proj();
    global_uniform.inv_proj = camera.get_inv_proj();
    global_uniform.camera_pos = camera.get_pos();
    global_uniform.resolution = {Window::width(), Window::height()};
    size_t offset = global_uniform_buffer.push(global_uniform);
    DescriptorSet::global_set.update_dynamic_offset(0, offset);

    cmd.set_viewport(VkContext::swapchain.default_viewport());
    cmd.set_scissor(VkContext::swapchain.default_scissor());

    {
        cmd.begin_renderpass(VkContext::renderpass, framebuffer);

        cmd.bind_pipeline(pipeline);
        cmd.bind_descriptor_set(pipeline, 0);
        cmd.bind_descriptor_set(pipeline, 1);

        cmd.draw(1);

        VkImgui::render_draw_data(cmd);

        cmd.end_renderpass();
    }

    VkContext::end_frame();
}

void VoxelApp::imgui_update()
{
    VkImgui::new_frame();

    VkImgui::window("ImGui", [&]() {
        if (ImGui::CollapsingHeader("Perf"))
        {
            ImGui::Text("Delta time: %lf ms (%u FPS)", Time::delta_time() * 1000.0, Time::fps());
        }
        if (ImGui::CollapsingHeader("Camera"))
        {
            static float speed = 50.0f;
            static float sensitivity = 0.1f;
            static float fov = 90.0f;

            const auto& pos = camera.get_pos();
            const auto& front = camera.get_front();
            const auto& up = camera.get_up();
            const auto& right = camera.get_right();
            ImGui::Text("Position | %7.2f | %7.2f | %7.2f |", pos.x, pos.y, pos.z);
            ImGui::Separator();
            ImGui::Text("Front    | %7.2f | %7.2f | %7.2f |", front.x, front.y, front.z);
            ImGui::Separator();
            ImGui::Text("Up       | %7.2f | %7.2f | %7.2f |", up.x, up.y, up.z);
            ImGui::Separator();
            ImGui::Text("Right    | %7.2f | %7.2f | %7.2f |", right.x, right.y, right.z);
            ImGui::Separator();
            if (ImGui::DragFloat("Speed", &speed, 0.1f, 0, 200))
            {
                camera.set_speed(speed);
            }
            if (ImGui::DragFloat("Sensitivity", &sensitivity, 0.001f, 0, 1))
            {
                camera.set_sensitivity(sensitivity);
            }
            if (ImGui::DragFloat("FOV", &fov, 0.1f, 0, 180))
            {
                camera.set_perspective(fov);
            }
        }
    });

    VkImgui::render();
}

void VoxelApp::key_callback(const Event& event, void* object)
{
    VoxelApp& test_app = *reinterpret_cast<VoxelApp*>(object);
    if (event.key() == GLFW_KEY_LEFT_ALT && event.key_action() == GLFW_PRESS)
    {
        Input::show_cursor(!Input::cursor_enabled());
        auto [x, y] = Input::get_cursor_pos();
        test_app.camera.set_last_x_y(x, y);
        return;
    }
    test_app.camera.on_key_event(event.key(), event.key_action());
}

void VoxelApp::cursor_pos_callback(const Event& event, void* object)
{
    VoxelApp& test_app = *reinterpret_cast<VoxelApp*>(object);
    if (!Input::cursor_enabled())
    {
        test_app.camera.on_mouse_moved(event.pos_x(), event.pos_y());
    }
}
