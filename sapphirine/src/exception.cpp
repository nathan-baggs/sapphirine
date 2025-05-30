#include "sapphirine/exception.h"

#include <cstdint>
#include <format>
#include <string_view>

namespace sapphirine
{

auto Exception::stacktrace() const -> std::string
{
    return std::to_string(trace_);
}

auto Exception::what() const -> std::string_view
{
    return what_;
}

auto Exception::to_string() const -> std::string
{
    return std::format("{}\n{}", what(), stacktrace());
}
}
