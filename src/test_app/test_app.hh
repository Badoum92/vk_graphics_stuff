#pragma once

#include "vk_context/vk_context.hh"
#include "buffer/buffer.hh"
#include "pipeline/pipeline.hh"
#include "image/image.hh"
#include "sampler/sampler.hh"

#include "event/event.hh"
#include "camera/camera.hh"

class TestApp
{
public:
    TestApp();
    ~TestApp();

    void update();

    static void key_callback(const Event& event, void* object);
    static void cursor_pos_callback(const Event& event, void* object);

private:
    Buffer vertex_buffer;
    Buffer index_buffer;
    Buffer global_uniform_buffer;

    Image image;
    Sampler sampler;

    Pipeline pipeline;

    Camera camera;
};
