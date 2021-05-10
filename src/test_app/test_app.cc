#include "test_app.hh"

#include <iostream>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec2 pos;
    glm::vec2 tex_coord;
};

TestApp::TestApp()
{
    // clang-format off
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f}, {0.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2,
        2, 3, 0
    };
    // clang-format on
    vertex_buffer.create_storage(vertices.data(), vertices.size() * sizeof(Vertex));
    index_buffer.create_index(indices.data(), indices.size() * sizeof(uint16_t));
    global_uniform_buffer.create_uniform(4 * KB);

    pipeline.create("shaders/test.vert.spv", "shaders/test.frag.spv");

    image.create("../../image.jpg");
    sampler.create();

    pipeline.descriptor_set(0).bind_buffer(0, global_uniform_buffer, 16 * sizeof(float));
    pipeline.descriptor_set(0).update();

    pipeline.descriptor_set(1).bind_buffer(0, vertex_buffer);
    pipeline.descriptor_set(1).bind_image_sampler(1, image, sampler);
    pipeline.descriptor_set(1).update();
}

TestApp::~TestApp()
{
}

void TestApp::update()
{
    auto frame_data = VkContext::begin_frame();
    if (!frame_data.has_value())
        return;
    auto& [cmd, framebuffer] = frame_data.value();

    cmd.set_viewport(VkContext::swapchain.default_viewport());
    cmd.set_scissor(VkContext::swapchain.default_scissor());

    size_t offset = global_uniform_buffer.push<glm::mat4>({{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}});
    DescriptorSet::global_set.update_dynamic_offset(0, offset);

    cmd.begin_renderpass(VkContext::renderpass, framebuffer);

    cmd.bind_pipeline(pipeline);
    cmd.bind_descriptor_set(pipeline, 0);
    cmd.bind_descriptor_set(pipeline, 1);

    cmd.bind_index_buffer(index_buffer, 0, VK_INDEX_TYPE_UINT16);

    cmd.draw_indexed(index_buffer.count<uint16_t>());

    cmd.end_renderpass();

    VkContext::end_frame();
}
