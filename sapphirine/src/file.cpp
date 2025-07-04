#include "sapphirine/file.h"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <span>
#include <string_view>

#include <Windows.h>

#include "sapphirine/auto_release.h"
#include "sapphirine/error.h"

namespace sapphirine
{
File::File(const std::filesystem::path &path, CreationMode mode, std::uint32_t size)
    : handle_{INVALID_HANDLE_VALUE, ::CloseHandle}
    , mapping_{NULL, ::CloseHandle}
    , map_view_{nullptr, ::UnmapViewOfFile}
    , size_{}
{
    expect(mode == CreationMode::OPEN || size > 0, "size must be greater than 0 when creating a file");

    handle_.reset(
        ::CreateFileA(
            path.string().c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            mode == CreationMode::OPEN ? OPEN_EXISTING : OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr));
    ensure(handle_, "failed to open file");

    mapping_.reset(::CreateFileMappingA(handle_, nullptr, PAGE_READWRITE, 0, size, nullptr));
    ensure(mapping_, "failed to map file: {}", ::GetLastError());

    map_view_.reset(::MapViewOfFile(mapping_, FILE_MAP_ALL_ACCESS, 0, 0, 0));
    ensure(map_view_, "failed to get map view");

    size_ = ::GetFileSize(handle_, nullptr);
}

auto File::as_string() const -> std::string_view
{
    return {reinterpret_cast<const char *>(map_view_.get()), size_};
}

auto File::as_data() const -> std::span<const std::byte>
{
    return {reinterpret_cast<const std::byte *>(map_view_.get()), size_};
}

}
