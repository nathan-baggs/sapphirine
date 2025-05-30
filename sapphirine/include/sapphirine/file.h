#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>
#include <ranges>
#include <span>
#include <string_view>

#include <Windows.h>

#include "sapphirine/auto_release.h"
#include "sapphirine/error.h"

namespace sapphirine
{
enum class CreationMode
{
    OPEN,
    CREATE
};

class File
{
  public:
    File(const std::filesystem::path &path, CreationMode mode = CreationMode::OPEN);

    auto as_string() const -> std::string_view;
    auto as_data() const -> std::span<const std::byte>;

    auto write(std::string_view data) -> void;
    auto write(std::span<const std::byte> data) -> void;

    template <class T>
    auto write(const T &data) -> void
        requires(sizeof(std::ranges::range_value_t<T>) == 1)
    {
        std::memcpy(map_view_.get(), std::ranges::data(data), std::ranges::size(data));
        ensure(::FlushViewOfFile(map_view_.get(), data.size()) != 0, "failed to flush file");
    }

  private:
    AutoRelease<HANDLE, INVALID_HANDLE_VALUE> handle_;
    AutoRelease<HANDLE, reinterpret_cast<HANDLE>(NULL)> mapping_;
    std::unique_ptr<void, decltype(&::UnmapViewOfFile)> map_view_;
    std::size_t size_;
};
}
