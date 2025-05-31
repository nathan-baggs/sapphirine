#pragma once

#include <cstddef>
#include <format>
#include <string>
#include <string_view>

namespace sapphirine
{

class Instruction
{
  public:
    Instruction(std::string_view mnemonic, std::string_view operands, std::size_t size);

    auto mnemonic() const -> std::string_view;
    auto operands() const -> std::string_view;
    auto size() const -> std::size_t;
    auto to_string() const -> std::string;

    auto operator==(const Instruction &other) const -> bool = default;

  private:
    std::string mnemonic_;
    std::string operands_;
    std::size_t size_;
};

}
