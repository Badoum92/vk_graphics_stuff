#include "vk/vk_shader.h"

#include "vk/vk_context.h"
#include "vk/vk_tools.h"

#include "core/core.h"
#include "core/math/math.h"
#include "core/file.h"
#include "core/log.h"
#include "core/thread_pool.h"
#include "core/str.h"
#include "core/thread.h"
#include "core/memory/linear_allocator.h"

#if defined(_WIN32)
#include <windows.h>
#endif

namespace vk
{
shader* context::create_shader(const char* path)
{
    linear_allocator* allocator = linear_allocator_get_global();

    file file = file_open_read(path);
    uint32_t size = (uint32_t)file_get_size(&file);
    uint8_t* data = (uint8_t*)linear_alloc(allocator, size);
    file_read(&file, data, size);
    file_close(&file);

    VkShaderModuleCreateInfo shader_info = {};
    shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_info.codeSize = size;
    shader_info.pCode = (uint32_t*)data;

    VkShaderModule vk_shader = VK_NULL_HANDLE;
    VK_CHECK(vkCreateShaderModule(device, &shader_info, nullptr, &vk_shader));
    linear_free(allocator, data);

    shader* shader = pool_alloc(&shaders);
    *shader = {vk_shader, path};
    return shader;
}

void context::destroy_shader(shader* shader)
{
    if (shader->vk_handle != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(device, shader->vk_handle, nullptr);
        shader->vk_handle = VK_NULL_HANDLE;
    }
    pool_free(&shaders, shader);
}
} // namespace vk

#if defined(_WIN32)
static int64_t latest_change(const char* dir_name)
{
    int64_t ret = 0;
    uint8_t file_info_buf[2048];
    HANDLE dir_handle = CreateFileA(dir_name, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                                    FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    FILE_INFO_BY_HANDLE_CLASS file_info_class = FileIdExtdDirectoryRestartInfo;
    while (true)
    {
        if (!GetFileInformationByHandleEx(dir_handle, file_info_class, file_info_buf, sizeof(file_info_buf)))
        {
            ASSERT(GetLastError() == ERROR_NO_MORE_FILES);
            break;
        }
        file_info_class = FileIdExtdDirectoryInfo;
        PFILE_ID_EXTD_DIR_INFO entry = (PFILE_ID_EXTD_DIR_INFO)file_info_buf;
        bool done = false;
        do
        {
            done = entry->NextEntryOffset == 0;
            if (wcsncmp(entry->FileName, L".", 1) == 0 || wcsncmp(entry->FileName, L"..", 2) == 0
                || (entry->FileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                goto loop;

            if (entry->ChangeTime.QuadPart > ret)
                ret = entry->ChangeTime.QuadPart;

        loop:
            entry = (PFILE_ID_EXTD_DIR_INFO)((uint8_t*)entry + entry->NextEntryOffset);
        } while (!done);
    }
    return ret;
}

bool vk_compile_shaders()
{
    CreateDirectoryA("shaders/spv", nullptr);
    int64_t latest_src_change = MAX(latest_change("shaders"), latest_change("shaders/include"));
    int64_t latest_dst_change = latest_change("shaders/spv");
    if (latest_dst_change >= latest_src_change)
        return false;

    char cmd_line[MAX_PATH * 3];
    uint8_t file_info_buf[2048];
    char file_name[MAX_PATH * 2];
    HANDLE shader_dir = CreateFileA("shaders", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                    nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

    uint32_t total = 0;
    uint32_t successes = 0;
    STARTUPINFOA startup_info;
    memset(&startup_info, 0, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    PROCESS_INFORMATION process_infos[64];

    FILE_INFO_BY_HANDLE_CLASS file_info_class = FileIdExtdDirectoryRestartInfo;
    while (true)
    {
        if (!GetFileInformationByHandleEx(shader_dir, file_info_class, file_info_buf, sizeof(file_info_buf)))
        {
            ASSERT(GetLastError() == ERROR_NO_MORE_FILES);
            break;
        }
        file_info_class = FileIdExtdDirectoryInfo;
        PFILE_ID_EXTD_DIR_INFO entry = (PFILE_ID_EXTD_DIR_INFO)file_info_buf;
        bool done = false;
        do
        {
            done = entry->NextEntryOffset == 0;
            size_t written = 0;
            ASSERT(total < 64);
            if (wcsncmp(entry->FileName, L".", 1) == 0 || wcsncmp(entry->FileName, L"..", 2) == 0
                || (entry->FileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                goto loop;

            written = wcstombs(file_name, entry->FileName, entry->FileNameLength / 2);
            file_name[written] = 0;

            if (!str_ends_with(file_name, ".vert") && !str_ends_with(file_name, ".frag")
                && !str_ends_with(file_name, ".comp"))
                goto loop;

            snprintf(cmd_line, sizeof(cmd_line),
                     "glslc -g -I shaders/include --target-env=vulkan1.4 -std=460 shaders/%s -o shaders/spv/%s",
                     file_name, file_name);
            LOG_DEBUG("%s", cmd_line);

            if (CreateProcessA(nullptr, cmd_line, nullptr, nullptr, false, 0, nullptr, nullptr, &startup_info,
                               &process_infos[total]))
            {
                total++;
            }

        loop:
            entry = (PFILE_ID_EXTD_DIR_INFO)((uint8_t*)entry + entry->NextEntryOffset);
        } while (!done);
    }

    DWORD exit_code;
    for (uint32_t i = 0; i < total; ++i)
    {
        WaitForSingleObject(process_infos[i].hProcess, INFINITE);
        GetExitCodeProcess(process_infos[i].hProcess, &exit_code);
        CloseHandle(process_infos[i].hThread);
        CloseHandle(process_infos[i].hProcess);
        successes += exit_code == 0;
    }

    if (successes == total)
        LOG_INFO("Shader compilation: %u / %u", successes, total);
    else
        LOG_ERROR("Shader compilation: %u / %u", successes, total);

    return true;
}
#endif