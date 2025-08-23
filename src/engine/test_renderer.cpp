#include "test_renderer.h"

#include "camera.h"

#include "vk/vk_pipeline.h"
#include "vk/vk_image.h"
#include "vk/vk_buffer.h"

#include "core/math/math.h"
#include "core/log.h"
#include "core/time.h"

#include "ufbx/ufbx.h"

#include "imgui.h"

struct push_constant
{
    VkDeviceAddress vertex_buffer;
    VkDeviceAddress uniform_buffer;
};

struct vertex
{
    vec4f position;
    vec4f normal;
    vec2f uv;
    vec2f _padding;
};

struct uniform_buffer_data
{
    mat4f view_proj;
};

test_renderer test_renderer::create(vk::context* _context, uint32_t _width, uint32_t _height)
{
    test_renderer test_renderer;

    test_renderer.width = _width;
    test_renderer.height = _height;
    test_renderer.context = _context;

    test_renderer.vertex_shader = _context->create_shader("shaders/spv/test_triangle.vert");
    test_renderer.fragment_shader = _context->create_shader("shaders/spv/test_triangle.frag");

    vk::image* swapchain_image = _context->surface.images[0];

    vk::graphics_pipeline_description pipeline_desc = {};
    pipeline_desc.vertex_shader = test_renderer.vertex_shader;
    pipeline_desc.fragment_shader = test_renderer.fragment_shader;
    pipeline_desc.color_formats[pipeline_desc.num_color_formats++] = swapchain_image->full_view.format;
    pipeline_desc.depth_format = VK_FORMAT_D32_SFLOAT;
    pipeline_desc.push_constant_size = sizeof(push_constant);
    pipeline_desc.name = "test pipeline";
    test_renderer.graphics_pipeline = _context->create_graphics_pipeline(pipeline_desc);

    /*uint32_t indices[] = {
        0,  3,  2,  2,  1,  0,  4,  5,  6,  6,  7,  4,  11, 8,  9,  9,  10, 11,
        12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20,
    };
    vertex vertices[] = {
        {{-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    };

    vk::buffer_description buffer_description = {};
    buffer_description.size = sizeof(uniform_buffer_data);
    buffer_description.usage = vk::uniform_buffer_usage;
    buffer_description.name = "uniform buffer";
    test_renderer.uniform_buffer_handle = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(indices);
    buffer_description.usage = vk::index_buffer_usage;
    buffer_description.name = "index buffer";
    test_renderer.index_buffer_handle = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(vertices);
    buffer_description.usage = vk::storage_buffer_usage;
    buffer_description.name = "vertex buffer";
    test_renderer.vertex_buffer_handle = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(indices);
    buffer_description.usage = vk::transfer_buffer_usage;
    buffer_description.name = "staging buffer";
    vk::buffer* index_staging_buffer = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(vertices);
    buffer_description.usage = vk::transfer_buffer_usage;
    buffer_description.name = "staging buffer";
    vk::buffer* vertex_staging_buffer = _context->create_buffer(buffer_description);

    vk::image_description image_description = {};
    image_description.width = width;
    image_description.height = height;
    image_description.format = VK_FORMAT_D32_SFLOAT;
    image_description.usage = vk::image_usage_depth_attachment;
    image_description.name = "depth image";
    test_renderer.depth_handle = _context->create_image(image_description);

    vk::command_buffer* cmd = _context->transfer_commands.get_command_buffer();
    cmd->upload_buffer(test_renderer.index_buffer_handle, index_staging_buffer, indices, sizeof(indices));
    cmd->upload_buffer(test_renderer.vertex_buffer_handle, vertex_staging_buffer, vertices, sizeof(vertices));
    cmd->barrier(test_renderer.depth_handle, vk::image_usage::depth_attachment);
    _context->submit(cmd);
    _context->wait_idle();
    _context->destroy_buffer(index_staging_buffer);
    _context->destroy_buffer(vertex_staging_buffer);*/

    ufbx_load_opts opts = {}; // Optional, pass NULL for defaults
    ufbx_error error; // Optional, pass NULL if you don't care about errors
    // ufbx_scene* scene = ufbx_load_file("resources/cube.fbx", &opts, &error);
    ufbx_scene* scene = ufbx_load_file("resources/sponza.fbx", &opts, &error);
    if (!scene)
    {
        fprintf(stderr, "Failed to load: %s\n", error.description.data);
        exit(1);
    }

    // Use and inspect `scene`, it's just plain data!

    // Let's just list all objects within the scene for example:
    for (uint32_t i = 0; i < scene->nodes.count; i++)
    {
        ufbx_node* node = scene->nodes.data[i];
        if (node->is_root)
            continue;

        if (!node->mesh)
        {
            continue;
        }

        ufbx_mesh* mesh = node->mesh;

        // Count the number of needed parts and temporary buffers
        size_t max_triangles = 0;

        // We need to render each material of the mesh in a separate part, so let's
        // count the number of parts and maximum number of triangles needed.
        for (uint32_t pi = 0; pi < mesh->material_parts.count; pi++)
        {
            ufbx_mesh_part* part = &mesh->material_parts.data[pi];
            if (part->num_triangles == 0)
                continue;
            max_triangles = MAX(max_triangles, part->num_triangles);
        }

        /* for (uint32_t j = 0; j < mesh->materials.count; ++j)
        {
            ufbx_material* material = mesh->materials[j];
            for (uint32_t k = 0; k < material->textures.count; ++k)
            {
                ufbx_texture* texture = material->textures[k].texture;
            }
        } */

        size_t num_tri_indices = mesh->max_face_triangles * 3;
        uint32_t* tri_indices = (uint32_t*)malloc(num_tri_indices * sizeof(uint32_t));
        vertex* vertices = (vertex*)malloc(max_triangles * 3 * sizeof(vertex));
        uint32_t* indices = (uint32_t*)malloc(max_triangles * 3 * sizeof(uint32_t));

        for (uint32_t pi = 0; pi < mesh->material_parts.count; pi++)
        {
            ufbx_mesh_part* mesh_part = &mesh->material_parts.data[pi];
            if (mesh_part->num_triangles == 0)
                continue;

            uint32_t num_indices = 0;

            for (uint32_t fi = 0; fi < mesh_part->num_faces; fi++)
            {
                ufbx_face face = mesh->faces.data[mesh_part->face_indices.data[fi]];
                uint32_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

                ufbx_vec2 default_uv = {};

                // Iterate through every vertex of every triangle in the triangulated result
                for (uint32_t vi = 0; vi < num_tris * 3; vi++)
                {
                    uint32_t ix = tri_indices[vi];
                    vertex* vert = &vertices[num_indices];

                    ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, ix);
                    ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, ix);
                    ufbx_vec2 uv = mesh->vertex_uv.exists ? ufbx_get_vertex_vec2(&mesh->vertex_uv, ix) : default_uv;

                    vert->position = {pos.x, pos.z, pos.y, 1.0f};
                    vert->normal = {normal.x, normal.z, normal.y, 1.0f};
                    vert->uv = {uv.x, uv.y};

                    num_indices++;
                }
            }

            ufbx_vertex_stream streams[1];
            uint32_t num_streams = 1;

            streams[0].data = vertices;
            streams[0].vertex_count = num_indices;
            streams[0].vertex_size = sizeof(vertex);

            uint32_t num_vertices =
                (uint32_t)ufbx_generate_indices(streams, num_streams, indices, num_indices, NULL, &error);
            ASSERT(error.type == UFBX_ERROR_NONE);

            vk::buffer_description buffer_description = {};
            buffer_description.size = num_indices * sizeof(uint32_t);
            buffer_description.usage = vk::index_buffer_usage;
            buffer_description.name = "index buffer";
            vk::buffer* index_buffer =
                test_renderer.index_buffers.push_back(_context->create_buffer(buffer_description));

            buffer_description.size = num_vertices * sizeof(vertex);
            buffer_description.usage = vk::storage_buffer_usage;
            buffer_description.name = "vertex buffer";
            vk::buffer* vertex_buffer =
                test_renderer.vertex_buffers.push_back(_context->create_buffer(buffer_description));

            buffer_description.size = num_indices * sizeof(uint32_t);
            buffer_description.usage = vk::transfer_buffer_usage;
            buffer_description.name = "staging buffer";
            vk::buffer* index_staging_buffer = _context->create_buffer(buffer_description);

            buffer_description.size = num_vertices * sizeof(vertex);
            buffer_description.usage = vk::transfer_buffer_usage;
            buffer_description.name = "staging buffer";
            vk::buffer* vertex_staging_buffer = _context->create_buffer(buffer_description);

            vk::command_buffer* cmd = _context->transfer_commands.get_command_buffer();
            cmd->upload_buffer(index_buffer, index_staging_buffer, indices, num_indices * sizeof(uint32_t));
            cmd->upload_buffer(vertex_buffer, vertex_staging_buffer, vertices, num_vertices * sizeof(vertex));
            _context->submit(cmd);
            _context->wait_idle();
            _context->destroy_buffer(index_staging_buffer);
            _context->destroy_buffer(vertex_staging_buffer);
        }
    }

    ufbx_free_scene(scene);

    vk::buffer_description buffer_description = {};
    buffer_description.size = sizeof(uniform_buffer_data);
    buffer_description.usage = vk::uniform_buffer_usage;
    buffer_description.name = "uniform buffer";
    test_renderer.uniform_buffer = _context->create_buffer(buffer_description);

    vk::image_description image_description = {};
    image_description.width = test_renderer.width;
    image_description.height = test_renderer.height;
    image_description.format = VK_FORMAT_D32_SFLOAT;
    image_description.usage = vk::image_usage_depth_attachment;
    image_description.name = "depth image";
    test_renderer.depth = _context->create_image(image_description);

    image_description.format = _context->surface.images[0]->description.format;
    image_description.usage = vk::image_usage_color_attachment;
    image_description.name = "imgui render target";
    test_renderer.render_target.image = _context->create_image(image_description);
    test_renderer.render_target.sampler = _context->default_sampler;
    imgui_add_texture(&test_renderer.render_target);

    vk::command_buffer* cmd = _context->transfer_commands.get_command_buffer();
    cmd->barrier(test_renderer.depth, vk::image_usage::depth_attachment);
    _context->submit(cmd);
    _context->wait_idle();

    return test_renderer;
}

void test_renderer::destroy()
{
    context->destroy_graphics_pipeline(graphics_pipeline);
    context->destroy_shader(fragment_shader);
    context->destroy_shader(vertex_shader);
    context->destroy_buffer(uniform_buffer);
    // context->destroy_buffer(index_buffer);
    // context->destroy_buffer(vertex_buffer);
    for (vk::buffer* buffer : index_buffers)
    {
        context->destroy_buffer(buffer);
    }
    for (vk::buffer* buffer : vertex_buffers)
    {
        context->destroy_buffer(buffer);
    }
    context->destroy_image(depth);
    imgui_remove_texture(&render_target);
    context->destroy_image(render_target.image);
}

void test_renderer::resize(uint32_t _width, uint32_t _height)
{
    if (_width == width && _height == height)
    {
        return;
    }

    context->wait_idle();

    width = _width;
    height = _height;

    vk::image_description image_description = {};
    image_description.width = width;
    image_description.height = height;
    image_description.format = VK_FORMAT_D32_SFLOAT;
    image_description.usage = vk::image_usage_depth_attachment;
    image_description.name = "depth image";
    context->destroy_image(depth);
    depth = context->create_image(image_description);

    image_description.format = context->surface.images[0]->description.format;
    image_description.usage = vk::image_usage_color_attachment;
    image_description.name = "imgui render target";
    imgui_remove_texture(&render_target);
    context->destroy_image(render_target.image);
    render_target.image = context->create_image(image_description);
    imgui_add_texture(&render_target);
}

void test_renderer::reload_shaders()
{
    context->wait_idle();
    context->destroy_graphics_pipeline(graphics_pipeline);
    context->destroy_shader(vertex_shader);
    context->destroy_shader(fragment_shader);

    vertex_shader = context->create_shader("shaders/spv/test_triangle.vert");
    fragment_shader = context->create_shader("shaders/spv/test_triangle.frag");

    vk::image* swapchain_image = context->surface.images[0];

    vk::graphics_pipeline_description pipeline_desc = {};
    pipeline_desc.vertex_shader = vertex_shader;
    pipeline_desc.fragment_shader = fragment_shader;
    pipeline_desc.color_formats[pipeline_desc.num_color_formats++] = swapchain_image->full_view.format;
    pipeline_desc.depth_format = VK_FORMAT_D32_SFLOAT;
    pipeline_desc.push_constant_size = sizeof(push_constant);
    pipeline_desc.name = "test pipeline";
    graphics_pipeline = context->create_graphics_pipeline(pipeline_desc);
}

void test_renderer::draw(vk::frame_context* frame_context, camera* camera)
{
    vk::command_buffer* command_buffer = frame_context->command_buffer;

    uniform_buffer_data uniform_buffer_data = {};
    uniform_buffer_data.view_proj = camera->view_proj;
    memcpy(uniform_buffer->mapped_data, &uniform_buffer_data, sizeof(uniform_buffer_data));

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    // scissor.extent = {width, height};
    scissor.extent = {render_target.image->description.width, render_target.image->description.height};
    command_buffer->set_scissor(scissor);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)render_target.image->description.width;
    viewport.height = (float)render_target.image->description.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    command_buffer->set_viewport(viewport);

    command_buffer->barrier(render_target.image, vk::image_usage::color_attachment);
    command_buffer->begin_rendering({{render_target.image}}, {{vk::load_op::clear_color()}}, depth,
                                    vk::load_op::clear_depth());

    vk::graphics_state graphics_state = vk::graphics_state::create();
    command_buffer->bind_graphics_pipeline(graphics_pipeline, graphics_state);
    command_buffer->bind_descriptor_buffer(graphics_pipeline);

    for (uint32_t i = 0; i < index_buffers.size; ++i)
    {
        push_constant push_constant;
        push_constant.vertex_buffer = vertex_buffers[i]->device_address;
        push_constant.uniform_buffer = uniform_buffer->device_address;
        command_buffer->push_constant(graphics_pipeline, &push_constant, sizeof(push_constant));

        command_buffer->bind_index_buffer(index_buffers[i]);
        command_buffer->draw_indexed((uint32_t)index_buffers[i]->description.size / sizeof(uint32_t));
    }

    command_buffer->end_rendering();

    command_buffer->barrier(render_target.image, vk::image_usage::compute_shader_read);
}
