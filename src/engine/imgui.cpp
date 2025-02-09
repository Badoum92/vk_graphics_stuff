#include "imgui.h"

#include "vk/vk_context.h"
#include "bul/containers/vector.h"
#include "bul/input.h"
#include "bul/log.h"

ImGuiID imgui_global_dockspace;

struct log_data
{
    bul::log_level log_level;
    uint32_t text_size;
    uint32_t text_capacity;
    int line;
    const char* file;
    char* text;
    char time[16];
};

static constexpr uint32_t max_logs = 128;
static log_data logs[max_logs];
static uint32_t log_index = 0;
static uint32_t log_count = 0;

static void imgui_log(bul::log_level level, const char* time, const char* file, int line, const char* fmt,
                      va_list va_args)
{
    log_data* log_data = &logs[log_index];
    log_index = (log_index + 1) % max_logs;

    log_data->log_level = level;
    strncpy(log_data->time, time, sizeof(log_data->time));
    log_data->line = line;
    log_data->file = file;
    va_list args_copy;
    va_copy(args_copy, va_args);
    uint32_t text_size = vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);
    if (text_size >= log_data->text_capacity)
    {
        free(log_data->text);
        log_data->text = (char*)malloc(text_size + 1);
        log_data->text_capacity = text_size;
    }
    log_data->text_size = text_size;
    vsnprintf(log_data->text, text_size + 1, fmt, va_args);
    va_end(va_args);

    log_count += log_count < max_logs;
}

static void log_window()
{
    if (ImGui::Begin("Logs"))
    {
        if (ImGui::Button("Clear"))
        {
            for (log_data& log_data : logs)
            {
                log_data.text_size = 0;
            }
            log_count = 0;
        }
        ImGui::SameLine();
        if (ImGui::Button("Test"))
        {
            log_debug("debug");
            log_info("info");
            log_warning("warn");
            log_error("error");
        }

        ImGui::BeginChild("Text");
        for (uint32_t i = 0; i < log_count; ++i)
        {
            log_data* log_data = &logs[(log_index - log_count + max_logs + i) % max_logs];

            if (log_data->text_size == 0)
            {
                continue;
            }

            ImVec4 level_color = {1, 1, 1, 1};
            const char* level_text = "";
            switch (log_data->log_level)
            {
            case bul::log_level_debug:
                level_color = {0.16f, 0.65f, 0.93f, 1.0f};
                level_text = "[DEBUG]";
                break;
            case bul::log_level_info:
                level_color = {0.15f, 0.93f, 0.30f, 1.0f};
                level_text = "[INFO] ";
                break;
            case bul::log_level_warning:
                level_color = {0.96f, 0.8f, 0.09f, 1.0f};
                level_text = "[WARN] ";
                break;
            case bul::log_level_error:
                level_color = {0.9f, 0.2f, 0.2f, 1.0f};
                level_text = "[ERROR]";
                break;
            default:
                break;
            }

            ImGui::Text("[%s] ", log_data->time);
            ImGui::SameLine();
            ImGui::TextColored(level_color, "%s ", level_text);
            ImGui::SameLine();
            ImGui::Text("%s", log_data->text);
            ImGui::SetItemTooltip("%s(%d)", log_data->file, log_data->line);
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void imgui_init(vk::context* context, bul::window* window)
{
    memset(logs, 0, sizeof(logs));
    bul::add_log_function(imgui_log);

    ImGui::CreateContext();
    ImGui_ImplVulkan_InitInfo imgui_vulkan = {};
    imgui_vulkan.Instance = context->instance;
    imgui_vulkan.PhysicalDevice = context->physical_device;
    imgui_vulkan.Device = context->device;
    imgui_vulkan.Queue = context->graphics_queue;
    imgui_vulkan.DescriptorPool = context->descriptor_pool;
    imgui_vulkan.MinImageCount = context->surface.images.size;
    imgui_vulkan.ImageCount = context->surface.images.size;
    imgui_vulkan.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    imgui_vulkan.UseDynamicRendering = true;
    imgui_vulkan.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    imgui_vulkan.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    imgui_vulkan.PipelineRenderingCreateInfo.pColorAttachmentFormats =
        &context->images.get(context->surface.images[0]).description.format;
    ImGui_ImplVulkan_Init(&imgui_vulkan);
    ImGui_ImplWin32_Init(window->handle);
    ImGui::GetIO().Fonts->AddFontFromFileTTF("resources/CascadiaCode.ttf", 15);
    ImGui_ImplVulkan_CreateFontsTexture();
    ImGui::GetIO().DisplaySize.x = (float)window->size.x;
    ImGui::GetIO().DisplaySize.y = (float)window->size.y;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    // ImGuiStyle& style = ImGui::GetStyle();
    // style.WindowMenuButtonPosition = ImGuiDir_None;
    // style.WindowBorderSize = 0.0f;
    // style.GrabRounding = 4.0f;
    // style.WindowRounding = 6.0f;
    // style.FrameRounding = 4.0f;
    // style.FramePadding = ImVec2(5.0f, 5.0f);
    // style.PopupBorderSize = 0.0f;
    // style.PopupRounding = 4.0f;
    // style.SeparatorTextPadding = ImVec2(5.0f, 5.0f);
    // style.TabBarBorderSize = 2.0f;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.50f, 0.50f, 0.80f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.51f, 0.51f, 0.51f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.43f, 0.43f, 0.43f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.50f, 0.50f, 0.50f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.95f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.55f, 0.55f, 0.55f, 0.50f);
    colors[ImGuiCol_Tab] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TabSelected] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_TabDimmed] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.66f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.77f, 0.39f, 0.03f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void imgui_shutdown()
{
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplVulkan_Shutdown();
    ImGui::DestroyContext();

    for (log_data& log_data : logs)
    {
        if (log_data.text_size != 0)
        {
            free(log_data.text);
        }
    }
}

void imgui_begin_frame()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    imgui_global_dockspace = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
}

void imgui_end_frame(vk::command_buffer* command_buffer)
{
    static bool show_demo_window = false;
    if (bul::key_pressed(bul::key::F2))
    {
        show_demo_window = !show_demo_window;
    }
    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    log_window();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer->vk_handle);
}

imgui_docknode imgui_docknode::begin(uint32_t _id)
{
    imgui_docknode node{_id};
    return node;
}

imgui_docknode imgui_docknode::begin_new(uint32_t _id)
{
    imgui_docknode node{_id};
    ImGui::DockBuilderRemoveNode(_id);
    ImGui::DockBuilderAddNode(_id, ImGuiDockNodeFlags_None);
    return node;
}

void imgui_docknode::end()
{
    ImGui::DockBuilderFinish(id);
}

void imgui_docknode::dock_window(const char* name)
{
    ImGui::DockBuilderDockWindow(name, id);
}

imgui_docknode_split_h imgui_docknode::split_h(float a_fRatio)
{
    imgui_docknode_split_h split;
    ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, a_fRatio, &split.left.id, &split.right.id);
    return split;
}

imgui_docknode_split_v imgui_docknode::split_v(float a_fRatio)
{
    imgui_docknode_split_v split;
    ImGui::DockBuilderSplitNode(id, ImGuiDir_Up, a_fRatio, &split.up.id, &split.down.id);
    return split;
}
