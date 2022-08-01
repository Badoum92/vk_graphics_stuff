#include "bul/util.h"

#include <Windows.h>

#include "bul/bul.h"

namespace bul
{
std::wstring to_utf16(const std::string_view str)
{
    if (str.empty())
    {
        return {};
    }

    int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
    ASSERT(size > 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), wstr.data(), size);
    return wstr;
}

std::string from_utf16(const std::wstring_view wstr)
{
    if (wstr.empty())
    {
        return {};
    }

    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    ASSERT(size > 0);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), str.data(), size, nullptr, nullptr);
    return str;
}
} // namespace bul
