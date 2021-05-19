#include "test_app.hh"

#include <iostream>
#include <glm/glm.hpp>

#include "input/input.hh"
#include "time/time.hh"
#include "vk_imgui/vk_imgui.hh"

struct Vertex
{
    glm::vec3 pos;
    float _pad0;
    glm::vec2 tex_coord;
    glm::vec2 _pad1;
};

struct GlobalUniform
{
    glm::mat4 view;
    glm::mat4 inv_view;
    glm::mat4 proj;
    glm::mat4 inv_proj;
};

TestApp::TestApp()
{
    EventHandler::register_key_callback(this);
    EventHandler::register_cursor_pos_callback(this);

    // clang-format off
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, 1.0f}, 0.0f, {1.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 1.0f}, 0.0f, {0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 1.0f}, 0.0f, {0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, 1.0f}, 0.0f, {1.0f, 0.0f}, {0.0f, 0.0f}},

        {{-1.8f, -0.8f, 0.5f}, 0.0f, {1.0f, 1.0f}, {0.0f, 0.0f}},
        {{-0.8f, -0.8f, 0.5f}, 0.0f, {0.0f, 1.0f}, {0.0f, 0.0f}},
        {{-0.8f, 0.2f, 0.5f}, 0.0f, {0.0f, 0.0f}, {0.0f, 0.0f}},
        {{-1.8f, 0.2f, 0.5f}, 0.0f, {1.0f, 0.0f}, {0.0f, 0.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2,
        2, 3, 0,

        4, 5, 6,
        6, 7, 4
    };
    // clang-format on
    vertex_buffer.create_storage(vertices.data(), vertices.size() * sizeof(Vertex));
    index_buffer.create_index(indices.data(), indices.size() * sizeof(uint16_t));
    global_uniform_buffer.create_uniform(4 * KB);

    pipeline.create("shaders/test.vert.spv", "shaders/test.frag.spv");

    image.create("../../image.jpg");
    sampler.create();

    pipeline.descriptor_set(0).bind_buffer(0, global_uniform_buffer, sizeof(GlobalUniform));
    pipeline.descriptor_set(0).update();

    pipeline.descriptor_set(1).bind_buffer(0, vertex_buffer);
    pipeline.descriptor_set(1).bind_image_sampler(1, image, sampler);
    pipeline.descriptor_set(1).update();
}

TestApp::~TestApp()
{}

void TestApp::update()
{
    camera.update();

    imgui_update();

    auto frame_data = VkContext::begin_frame();
    if (!frame_data.has_value())
        return;

    auto& [cmd, framebuffer] = frame_data.value();

    cmd.set_viewport(VkContext::swapchain.default_viewport());
    cmd.set_scissor(VkContext::swapchain.default_scissor());

    GlobalUniform global_uniform{camera.get_view(), camera.get_inv_view(), camera.get_proj(), camera.get_inv_proj()};
    size_t offset = global_uniform_buffer.push(global_uniform);
    DescriptorSet::global_set.update_dynamic_offset(0, offset);

    {
        cmd.begin_renderpass(VkContext::renderpass, framebuffer);

        cmd.bind_pipeline(pipeline);
        cmd.bind_descriptor_set(pipeline, 0);
        cmd.bind_descriptor_set(pipeline, 1);

        cmd.bind_index_buffer(index_buffer, 0, VK_INDEX_TYPE_UINT16);
        cmd.draw_indexed(index_buffer.count<uint16_t>());

        VkImgui::render_draw_data(cmd);

        cmd.end_renderpass();
    }

    VkContext::end_frame();
}

void TestApp::imgui_update()
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

void TestApp::key_callback(const Event& event, void* object)
{
    TestApp& test_app = *reinterpret_cast<TestApp*>(object);
    if (event.key() == GLFW_KEY_LEFT_ALT && event.key_action() == GLFW_PRESS)
    {
        Input::show_cursor(!Input::cursor_enabled());
        auto [x, y] = Input::get_cursor_pos();
        test_app.camera.set_last_x_y(x, y);
        return;
    }
    test_app.camera.on_key_event(event.key(), event.key_action());
}

void TestApp::cursor_pos_callback(const Event& event, void* object)
{
    TestApp& test_app = *reinterpret_cast<TestApp*>(object);
    if (!Input::cursor_enabled())
    {
        test_app.camera.on_mouse_moved(event.pos_x(), event.pos_y());
    }
}
