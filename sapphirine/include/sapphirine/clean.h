#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>

namespace sapphirine
{

struct JumpChain
{
    std::size_t start;
    std::size_t consumed;
    std::size_t jump_count;

    auto to_string() const -> std::string;
};

auto clean(std::span<const std::byte> data, std::size_t virtual_address) -> std::vector<JumpChain>;

}
