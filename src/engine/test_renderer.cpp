#include "test_renderer.h"

#include "camera.h"

#include "bul/math/math.h"
#include "bul/log.h"
#include "ufbx/ufbx.h"

struct push_constant
{
    VkDeviceAddress vertex_buffer;
    VkDeviceAddress uniform_buffer;
};

struct vertex
{
    bul::vec4f position;
    bul::vec4f normal;
    bul::vec2f uv;
    bul::vec2f _padding;
};

struct uniform_buffer_data
{
    bul::mat4f view_proj;
};

test_renderer test_renderer::create(vk::context* _context)
{
    test_renderer test_renderer;

    test_renderer.context = _context;

    test_renderer.vertex_shader = _context->create_shader("shaders/test_triangle.vert.spv");
    test_renderer.fragment_shader = _context->create_shader("shaders/test_triangle.frag.spv");

    vk::image swapchain_image = _context->images.get(_context->surface.images[0]);

    vk::graphics_pipeline_description graphics_pipeline_description = {};
    graphics_pipeline_description.vertex_shader = test_renderer.vertex_shader;
    graphics_pipeline_description.fragment_shader = test_renderer.fragment_shader;
    graphics_pipeline_description.color_attachment_formats.push_back(swapchain_image.full_view.format);
    graphics_pipeline_description.depth_attachment_format = VK_FORMAT_D32_SFLOAT;
    graphics_pipeline_description.push_constant_size = sizeof(push_constant);
    graphics_pipeline_description.name = "test triangle";
    test_renderer.graphics_pipeline_handle = _context->create_graphics_pipeline(graphics_pipeline_description);

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
    bul::handle<vk::buffer> index_staging_buffer = _context->create_buffer(buffer_description);

    buffer_description.size = sizeof(vertices);
    buffer_description.usage = vk::transfer_buffer_usage;
    buffer_description.name = "staging buffer";
    bul::handle<vk::buffer> vertex_staging_buffer = _context->create_buffer(buffer_description);

    vk::image_description image_description = {};
    image_description.width = _context->surface.extent.width;
    image_description.height = _context->surface.extent.height;
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

    ufbx_load_opts opts = {0}; // Optional, pass NULL for defaults
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
    for (size_t i = 0; i < scene->nodes.count; i++)
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
        size_t max_parts = 0;
        size_t max_triangles = 0;

        // We need to render each material of the mesh in a separate part, so let's
        // count the number of parts and maximum number of triangles needed.
        for (size_t pi = 0; pi < mesh->material_parts.count; pi++)
        {
            ufbx_mesh_part* part = &mesh->material_parts.data[pi];
            if (part->num_triangles == 0)
                continue;
            max_parts += 1;
            max_triangles = bul_max(max_triangles, part->num_triangles);
        }

        for (size_t i = 0; i < mesh->materials.count; ++i)
        {
            ufbx_material* material = mesh->materials[i];
            for (size_t j = 0; j < material->textures.count; ++j)
            {
                ufbx_texture* texture = material->textures[j].texture;
            }
        }

        size_t num_tri_indices = mesh->max_face_triangles * 3;
        uint32_t* tri_indices = (uint32_t*)malloc(num_tri_indices * sizeof(uint32_t));
        vertex* vertices = (vertex*)malloc(max_triangles * 3 * sizeof(vertex));
        uint32_t* indices = (uint32_t*)malloc(max_triangles * 3 * sizeof(uint32_t));

        for (size_t pi = 0; pi < mesh->material_parts.count; pi++)
        {
            ufbx_mesh_part* mesh_part = &mesh->material_parts.data[pi];
            if (mesh_part->num_triangles == 0)
                continue;

            size_t num_indices = 0;

            for (size_t fi = 0; fi < mesh_part->num_faces; fi++)
            {
                ufbx_face face = mesh->faces.data[mesh_part->face_indices.data[fi]];
                size_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

                ufbx_vec2 default_uv = {0};

                // Iterate through every vertex of every triangle in the triangulated result
                for (size_t vi = 0; vi < num_tris * 3; vi++)
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
            size_t num_streams = 1;

            streams[0].data = vertices;
            streams[0].vertex_count = num_indices;
            streams[0].vertex_size = sizeof(vertex);

            size_t num_vertices = ufbx_generate_indices(streams, num_streams, indices, num_indices, NULL, &error);
            ASSERT(error.type == UFBX_ERROR_NONE);

            vk::buffer_description buffer_description = {};
            buffer_description.size = num_indices * sizeof(uint32_t);
            buffer_description.usage = vk::index_buffer_usage;
            buffer_description.name = "index buffer";
            auto& index_buffer_handle =
                test_renderer.index_buffer_handle.push_back(_context->create_buffer(buffer_description));

            buffer_description.size = num_vertices * sizeof(vertex);
            buffer_description.usage = vk::storage_buffer_usage;
            buffer_description.name = "vertex buffer";
            auto& vertex_buffer_handle =
                test_renderer.vertex_buffer_handle.push_back(_context->create_buffer(buffer_description));

            buffer_description.size = num_indices * sizeof(uint32_t);
            buffer_description.usage = vk::transfer_buffer_usage;
            buffer_description.name = "staging buffer";
            bul::handle<vk::buffer> index_staging_buffer = _context->create_buffer(buffer_description);

            buffer_description.size = num_vertices * sizeof(vertex);
            buffer_description.usage = vk::transfer_buffer_usage;
            buffer_description.name = "staging buffer";
            bul::handle<vk::buffer> vertex_staging_buffer = _context->create_buffer(buffer_description);

            vk::command_buffer* cmd = _context->transfer_commands.get_command_buffer();
            cmd->upload_buffer(index_buffer_handle, index_staging_buffer, indices, num_indices * sizeof(uint32_t));
            cmd->upload_buffer(vertex_buffer_handle, vertex_staging_buffer, vertices, num_vertices * sizeof(vertex));
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
    test_renderer.uniform_buffer_handle = _context->create_buffer(buffer_description);

    vk::image_description image_description = {};
    image_description.width = _context->surface.extent.width;
    image_description.height = _context->surface.extent.height;
    image_description.format = VK_FORMAT_D32_SFLOAT;
    image_description.usage = vk::image_usage_depth_attachment;
    image_description.name = "depth image";
    test_renderer.depth_handle = _context->create_image(image_description);
    vk::command_buffer* cmd = _context->transfer_commands.get_command_buffer();
    cmd->barrier(test_renderer.depth_handle, vk::image_usage::depth_attachment);
    _context->submit(cmd);
    _context->wait_idle();

    test_renderer.y_rotation_deg = 0;

    return test_renderer;
}

void test_renderer::destroy()
{
    context->destroy_graphics_pipeline(graphics_pipeline_handle);
    context->destroy_shader(fragment_shader);
    context->destroy_shader(vertex_shader);
    context->destroy_buffer(uniform_buffer_handle);
    // context->destroy_buffer(index_buffer_handle);
    // context->destroy_buffer(vertex_buffer_handle);
    for (bul::handle<vk::buffer> handle : index_buffer_handle)
    {
        context->destroy_buffer(handle);
    }
    for (bul::handle<vk::buffer> handle : vertex_buffer_handle)
    {
        context->destroy_buffer(handle);
    }
    context->destroy_image(depth_handle);
}

void test_renderer::resize()
{
    vk::image_description image_description = {};
    image_description.width = context->surface.extent.width;
    image_description.height = context->surface.extent.height;
    image_description.format = VK_FORMAT_D32_SFLOAT;
    image_description.usage = vk::image_usage_depth_attachment;
    image_description.name = "depth image";
    context->destroy_image(depth_handle);
    depth_handle = context->create_image(image_description);
}

void test_renderer::draw(vk::frame_context* frame_context, camera* camera, float delta_time)
{
    vk::command_buffer* command_buffer = frame_context->graphics_commands.get_command_buffer();

    // y_rotation_deg += 36 * delta_time;

    vk::buffer& uniform_buffer = context->buffers.get(uniform_buffer_handle);
    uniform_buffer_data uniform_buffer_data = {};
    uniform_buffer_data.view_proj = camera->proj * camera->view * bul::rotation_y(bul_radians(y_rotation_deg));
    memcpy(uniform_buffer.mapped_data, &uniform_buffer_data, sizeof(uniform_buffer_data));

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = context->surface.extent;
    command_buffer->set_scissor(scissor);

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = (float)context->surface.extent.width;
    viewport.height = (float)context->surface.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    command_buffer->set_viewport(viewport);

    command_buffer->barrier(frame_context->image, vk::image_usage::color_attachment);
    command_buffer->begin_rendering({{frame_context->image}}, {{vk::load_op::clear_color()}}, depth_handle,
                                    vk::load_op::clear_depth());

    vk::graphics_state graphics_state = vk::graphics_state::create();
    graphics_state.cull_back_faces = false;
    graphics_state.depth_compare_op = VK_COMPARE_OP_GREATER;
    command_buffer->bind_pipeline(graphics_pipeline_handle, graphics_state);

    command_buffer->bind_descriptor_buffer(graphics_pipeline_handle);

    for (uint32_t i = 0; i < index_buffer_handle.size; ++i)
    {
        push_constant push_constant;
        push_constant.vertex_buffer = context->buffers.get(vertex_buffer_handle[i]).device_address;
        push_constant.uniform_buffer = context->buffers.get(uniform_buffer_handle).device_address;
        command_buffer->push_constant(graphics_pipeline_handle, &push_constant, sizeof(push_constant));

        vk::buffer& index_buffer = context->buffers.get(index_buffer_handle[i]);
        command_buffer->bind_index_buffer(index_buffer_handle[i]);
        command_buffer->draw_indexed(index_buffer.description.size / sizeof(uint32_t));
    }

    command_buffer->end_rendering();

    command_buffer->barrier(frame_context->image, vk::image_usage::present);

    context->submit(command_buffer, frame_context);
}