#pragma once

#include <filesystem>
#include <stdexcept>
#include <fstream>

namespace Tools
{
    template <typename T>
    T read_file(const std::filesystem::path& path)
    {
        std::ifstream file{path, std::ios::binary | std::ios::ate};
        if (file.fail())
        {
            throw std::runtime_error("Could not read file " + path.string());
        }

        std::streampos end = file.tellg();
        file.seekg(0, std::ios::beg);
        std::streampos begin = file.tellg();

        T result;
        result.resize(static_cast<size_t>(end - begin) / sizeof(typename T::value_type));

        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(result.data()), end - begin);
        file.close();

        return result;
    }
}
