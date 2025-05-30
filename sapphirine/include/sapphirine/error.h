#pragma once

#include <format>
#include <memory>
#include <stacktrace>
#include <string_view>
#include <utility>

#include "sapphirine/auto_release.h"
#include "sapphirine/exception.h"
#include "sapphirine/formatter.h"
#include "sapphirine/log.h"

namespace sapphirine
{

#pragma warning(push)
#pragma warning(disable : 4702)
template <class... Args>
auto expect(bool predicate, std::format_string<Args...> msg, Args &&...args) -> void
{
    if (!predicate)
    {
        log::error("{}", std::format(msg, std::forward<Args>(args)...));
        log::error("{}", std::stacktrace::current(2));
        std::terminate();
        std::unreachable();
    }
}
#pragma warning(pop)

template <class... Args>
auto ensure(bool predicate, std::format_string<Args...> msg, Args &&...args) -> void
{
    if (!predicate)
    {
        throw Exception(msg, std::forward<Args>(args)...);
    }
}

template <class T, T Invalid, class... Args>
auto ensure(AutoRelease<T, Invalid> &obj, std::format_string<Args...> msg, Args &&...args) -> void
{
    ensure(!!obj, msg, std::forward<Args>(args)...);
}

template <class T, class D, class... Args>
auto ensure(std::unique_ptr<T, D> &obj, std::format_string<Args...> msg, Args &&...args) -> void
{
    ensure(!!obj, msg, std::forward<Args>(args)...);
}

}
