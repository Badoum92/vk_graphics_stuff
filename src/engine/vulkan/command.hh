#pragma once

#include <vector>
#include <type_traits>

#include <vulkan/vulkan.h>

#include "vk_tools.hh"
#include "handle.hh"

namespace vk
{
struct Device;
struct LoadOp;
struct FrameBuffer;
struct Buffer;
struct Image;
enum class ImageUsage;
struct DescriptorSet;
struct GraphicsProgram;
struct ComputeProgram;

struct Command
{
    void begin();
    void end();

    void barrier(const Handle<Image>& image_handle, ImageUsage dst_usage);

    void bind_image(const Handle<GraphicsProgram>& program_handle, const Handle<Image>& image_handle, uint32_t binding);
    void bind_uniform_buffer(const Handle<GraphicsProgram>& program_handle, const Handle<Buffer>& buffer_handle,
                             uint32_t binding, uint32_t offset, uint32_t size);
    void bind_storage_buffer(const Handle<GraphicsProgram>& program_handle, const Handle<Buffer>& buffer_handle,
                             uint32_t binding);
    void bind_image(const Handle<ComputeProgram>& program_handle, const Handle<Image>& image_handle, uint32_t binding);
    void bind_uniform_buffer(const Handle<ComputeProgram>& program_handle, const Handle<Buffer>& buffer_handle,
                             uint32_t binding, uint32_t offset, uint32_t size);
    void bind_storage_buffer(const Handle<ComputeProgram>& program_handle, const Handle<Buffer>& buffer_handle,
                             uint32_t binding);

    Device* p_device = nullptr;
    VkCommandBuffer vk_handle = VK_NULL_HANDLE;
    VkQueue vk_queue = VK_NULL_HANDLE;
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
};

struct TransferCommand : public Command
{
    void upload_buffer(const Handle<Buffer>& buffer_handle, void* data, uint32_t size);
    void upload_image(const Handle<Image>& image_handle, void* data, uint32_t size);
    void upload_image(const Handle<Image>& image_handle, const std::string& path);
    void blit_image(const Handle<Image>& src, const Handle<Image>& dst);
};

struct ComputeCommand : public TransferCommand
{
    void bind_descriptor_set(const Handle<ComputeProgram>& program_handle, DescriptorSet& set, uint32_t set_index);
    void bind_pipeline(const Handle<ComputeProgram>& program_handle);
    void dispatch(uint32_t x, uint32_t y, uint32_t z = 1);
};

struct GraphicsCommand : public ComputeCommand
{
    void set_scissor(const VkRect2D& rect);
    void set_viewport(const VkViewport& viewport);

    void bind_index_buffer(const Handle<Buffer>& buffer_handle, VkIndexType index_type, uint32_t offset);
    void bind_descriptor_set(const Handle<GraphicsProgram>& program_handle, DescriptorSet& set, uint32_t set_index);
    void bind_pipeline(const Handle<GraphicsProgram>& program_handle, uint32_t pipeline_index = 0);

    void begin_renderpass(const Handle<FrameBuffer>& framebuffer_handle, const std::vector<LoadOp>& load_ops);
    void end_renderpass();

    void draw(uint32_t vertex_count, uint32_t first_vertex = 0);
    void draw_indexed(uint32_t index_count, uint32_t first_index = 0, uint32_t vertex_offset = 0);

    using ComputeCommand::bind_descriptor_set;
    using ComputeCommand::bind_pipeline;
};

template <typename T>
struct CommandPool
{
    static CommandPool<T> create(Device& device)
    {
        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        if constexpr (std::is_same_v<GraphicsCommand, T>)
        {
            pool_info.queueFamilyIndex = device.graphics_family_index;
        }
        else if constexpr (std::is_same_v<ComputeCommand, T>)
        {
            pool_info.queueFamilyIndex = device.compute_family_index;
        }
        else if constexpr (std::is_same_v<TransferCommand, T>)
        {
            pool_info.queueFamilyIndex = device.transfer_family_index;
        }
        pool_info.flags = 0;

        CommandPool command_pool{};
        VK_CHECK(vkCreateCommandPool(device.vk_handle, &pool_info, nullptr, &command_pool.vk_handle));
        command_pool.p_device = &device;

        return command_pool;
    }

    void destroy()
    {
        reset();
        vkDestroyCommandPool(p_device->vk_handle, vk_handle, nullptr);
    }

    void reset()
    {
        VK_CHECK(vkResetCommandPool(p_device->vk_handle, vk_handle, 0));
        index = 0;
    }

    T& get_command()
    {
        if (index == commands.size())
        {
            VkCommandBufferAllocateInfo alloc_info{};
            alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            alloc_info.commandPool = vk_handle;
            alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            alloc_info.commandBufferCount = 1;

            T command;
            command.p_device = p_device;
            if constexpr (std::is_same_v<GraphicsCommand, T>)
            {
                command.vk_queue = p_device->graphics_queue;
            }
            else if constexpr (std::is_same_v<ComputeCommand, T>)
            {
                command.vk_queue = p_device->compute_queue;
            }
            else if constexpr (std::is_same_v<TransferCommand, T>)
            {
                command.vk_queue = p_device->transfer_queue;
            }

            VK_CHECK(vkAllocateCommandBuffers(p_device->vk_handle, &alloc_info, &command.vk_handle));
            commands.push_back(command);
        }
        T& cmd = commands[index++];
        cmd.begin();
        return cmd;
    }

    VkCommandPool vk_handle = VK_NULL_HANDLE;
    std::vector<T> commands;
    size_t index = 0;
    Device* p_device = nullptr;
};

struct CommandContext
{
    static CommandContext create(Device& device);
    void destroy();
    void reset();

    GraphicsCommand& get_graphics_command();
    ComputeCommand& get_compute_command();
    TransferCommand& get_transfer_command();

    CommandPool<GraphicsCommand> graphics_pool;
    CommandPool<ComputeCommand> compute_pool;
    CommandPool<TransferCommand> transfer_pool;

    Device* p_device = nullptr;
};
} // namespace vk
