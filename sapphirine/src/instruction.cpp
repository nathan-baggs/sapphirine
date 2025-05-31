#include "sapphirine/instruction.h"

#include <cstddef>
#include <string>
#include <string_view>

namespace sapphirine
{

Instruction::Instruction(std::string_view mnemonic, std::string_view operands, std::size_t size)
    : mnemonic_(mnemonic)
    , operands_(operands)
    , size_(size)
{
}

auto Instruction::mnemonic() const -> std::string_view
{
    return mnemonic_;
}

auto Instruction::operands() const -> std::string_view
{
    return operands_;
}

auto Instruction::size() const -> std::size_t
{
    return size_;
}

auto Instruction::to_string() const -> std::string
{
    return std::format("{} {} ({})", mnemonic_, operands_, size_);
}

}
