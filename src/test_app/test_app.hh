#pragma once

#include <application/application.hh>

#include "vk_context/vk_context.hh"
#include "buffer/buffer.hh"
#include "pipeline/pipeline.hh"
#include "image/image.hh"
#include "sampler/sampler.hh"

class TestApp : public Application
{
public:
    TestApp();
    ~TestApp();

    virtual void update() override;

private:
    Buffer vertex_buffer;
    Buffer index_buffer;
    Buffer global_uniform_buffer;

    Image image;
    Sampler sampler;

    Pipeline pipeline;
};
