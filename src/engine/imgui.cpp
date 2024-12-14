#include "imgui.h"

#include "vk/context.h"
#include "bul/containers/vector.h"
#include "bul/input.h"
#include "bul/log.h"

struct log_data
{
    bul::log_level log_level;
    char time[16];
    int line;
    const char* file;
    char* text;
    uint32_t text_size;
    uint32_t text_capacity;
};

static constexpr uint32_t max_logs = 128;
static log_data logs[max_logs];
static uint32_t log_index = 0;

static bool show_debug = true;
static bool show_info = true;
static bool show_warning = true;
static bool show_error = true;
static bool show_file = false;

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
}

static void _imgui_log_line(log_data* log_data)
{
    if (log_data->text_size == 0)
    {
        return;
    }

    ImVec4 level_color = {1, 1, 1, 1};
    const char* level_text = "";
    switch (log_data->log_level)
    {
    case bul::log_level_debug:
        if (!show_debug)
        {
            return;
        }
        level_color = {0, 0, 1, 1};
        level_text = "D";
        break;
    case bul::log_level_info:
        if (!show_info)
        {
            return;
        }
        level_color = {0, 1, 0, 1};
        level_text = "I";
        break;
    case bul::log_level_warning:
        if (!show_warning)
        {
            return;
        }
        level_color = {1, 1, 0, 1};
        level_text = "W";
        break;
    case bul::log_level_error:
        if (!show_error)
        {
            return;
        }
        level_color = {1, 0, 0, 1};
        level_text = "E";
        break;
    default:
        break;
    }

    ImGui::TableNextColumn();
    ImGui::Text("%s", log_data->time);
    ImGui::TableNextColumn();
    ImGui::TextColored(level_color, "%s", level_text);
    if (show_file)
    {
        ImGui::TableNextColumn();
        ImGui::Text("%s (%d)", log_data->file, log_data->line);
    }
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(log_data->text);
}

static void _imgui_log()
{
    ImGui::Begin("Logs");

    ImGui::BeginChild("show_file", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
    ImGui::Checkbox("Show file", &show_file);
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("filters", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
    ImGui::Checkbox("Debug", &show_debug);
    ImGui::SameLine();
    ImGui::Checkbox("Info", &show_info);
    ImGui::SameLine();
    ImGui::Checkbox("Warning", &show_warning);
    ImGui::SameLine();
    ImGui::Checkbox("Error", &show_error);
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("actions", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY);
    if (ImGui::Button("Clear"))
    {
        for (log_data& log_data : logs)
        {
            log_data.text_size = 0;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Test"))
    {
        log_info("test");
    }
    ImGui::EndChild();

    ImGui::BeginChild("logs");
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable
        | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody
        | ImGuiTableFlags_RowBg;
    if (ImGui::BeginTable("logs_table", 3 + show_file, flags))
    {
        ImGui::TableSetupColumn("time", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("level", ImGuiTableColumnFlags_WidthFixed);
        if (show_file)
        {
            ImGui::TableSetupColumn("file", ImGuiTableColumnFlags_WidthStretch);
        }
        ImGui::TableSetupColumn("text", ImGuiTableColumnFlags_WidthStretch);

        for (uint32_t i = (log_index + 1) % max_logs; i != log_index; i = (i + 1) % max_logs)
        {
            _imgui_log_line(&logs[i]);
        }
        _imgui_log_line(&logs[log_index]);
        ImGui::EndTable();
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::EndChild();
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
    ImGui_ImplVulkan_CreateFontsTexture();
    ImGui::GetIO().DisplaySize.x = (float)window->size.x;
    ImGui::GetIO().DisplaySize.y = (float)window->size.y;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowMenuButtonPosition = ImGuiDir_None;
    style->WindowBorderSize = 0.0f;
    style->WindowMenuButtonPosition = ImGuiDir::ImGuiDir_Right;
    style->GrabRounding = 4.0f;
    style->WindowRounding = 6.0f;
    style->FrameRounding = 4.0f;
    style->FramePadding = ImVec2(5.0f, 5.0f);
    style->PopupBorderSize = 0.0f;
    style->PopupRounding = 4.0f;
    style->SeparatorTextPadding = ImVec2(5.0f, 5.0f);
    style->TabBarBorderSize = 2.0f;

    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
    style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.0f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.03f, 0.03f, 0.03f, 0.8f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.03f, 0.03f, 0.03f, 1.0f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.0f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.769f, 0.392f, 0.031f, 0.8f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.769f, 0.392f, 0.031f, 1.0f);
    style->Colors[ImGuiCol_Tab] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    style->Colors[ImGuiCol_TabHovered] = ImVec4(0.552f, 0.552f, 0.552f, 0.5f);
    style->Colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
    style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.769f, 0.392f, 0.031f, 1.0f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.769f, 0.392f, 0.031f, 0.8f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.769f, 0.392f, 0.031f, 1.0f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.769f, 0.392f, 0.031f, 0.5f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.769f, 0.392f, 0.031f, 1.0f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.769f, 0.392f, 0.031f, 0.35f);
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
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
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

    _imgui_log();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer->vk_handle);
}
