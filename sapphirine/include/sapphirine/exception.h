#pragma once

#include <cstdint>
#include <format>
#include <stacktrace>
#include <string>

namespace sapphirine
{

class Exception
{
  public:
    template <class... Args>
    constexpr Exception(std::format_string<Args...> what, Args &&...args)
        : trace_(std::stacktrace::current(1))
        , what_(std::format(what, std::forward<Args>(args)...))
    {
    }

    template <class... Args>
    constexpr Exception(std::format_string<Args...> what, Args &&...args, std::uint32_t skip)
        : trace_(std::stacktrace::current(skip))
        , what_(std::format(what, std::forward<Args>(args)...))
    {
    }

    auto stacktrace() const -> std::string;

    auto to_string() const -> std::string;
    auto what() const -> std::string_view;

  private:
    std::string what_;
    std::stacktrace trace_;
};

}
