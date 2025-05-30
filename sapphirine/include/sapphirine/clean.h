#pragma once

#include <cstddef>
#include <span>

namespace sapphirine
{

auto clean(std::span<const std::byte> data) -> void;

}
