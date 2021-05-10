#pragma once

#include "vk_context/vk_context.hh"
#include "buffer/buffer.hh"
#include "pipeline/pipeline.hh"
#include "image/image.hh"
#include "sampler/sampler.hh"

#include "event/event.hh"

class TestApp
{
public:
    TestApp();
    ~TestApp();

    void update();

private:
    Buffer vertex_buffer;
    Buffer index_buffer;
    Buffer global_uniform_buffer;

    Image image;
    Sampler sampler;

    Pipeline pipeline;
};
