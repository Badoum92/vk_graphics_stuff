#pragma once

#include "buffer/buffer.hh"
#include "pipeline/pipeline.hh"
#include "image/image.hh"
#include "sampler/sampler.hh"

#include "event/event.hh"
#include "camera/camera.hh"

class VoxelApp
{
public:
    VoxelApp();
    ~VoxelApp();

    void update();
    void imgui_update();

    static void key_callback(const Event& event, void* object);
    static void cursor_pos_callback(const Event& event, void* object);

private:
    Buffer voxels;
    Buffer global_uniform_buffer;

    Buffer cube_vbo;
    Buffer cube_ebo;

    Pipeline pipeline;

    Camera camera;
};
