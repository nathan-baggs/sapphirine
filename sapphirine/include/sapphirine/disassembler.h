#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <vector>

#include "sapphirine/instruction.h"

namespace sapphirine
{

class Disassembler
{
  public:
    Disassembler();
    ~Disassembler();

    auto disassemble(std::span<const std::byte> data) const -> std::vector<Instruction>;

  private:
    struct implementation;
    std::unique_ptr<implementation> impl_;
};

}
