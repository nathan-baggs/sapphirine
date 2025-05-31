#include "sapphirine/disassembler.h"

#include <cstddef>
#include <memory>
#include <stdexcept>

#include <capstone/capstone.h>

#include "sapphirine/exception.h"

namespace
{

struct CapstoneDisasembler
{
    CapstoneDisasembler(::csh handle, std::span<const std::byte> data)
    {
        count =
            ::cs_disasm(handle, reinterpret_cast<const std::uint8_t *>(data.data()), data.size(), 0, 0, &instruction);
        if (count == 0)
        {
            throw sapphirine::Exception("failed to disassemble data {} bytes", data.size_bytes());
        }
    }

    ~CapstoneDisasembler()
    {
        ::cs_free(instruction, count);
    }

    ::cs_insn *instruction;
    std::size_t count;
};

}

namespace sapphirine
{

struct Disassembler::implementation
{
    ::csh handle;
};

Disassembler::Disassembler()
    : impl_(std::make_unique<implementation>())
{
    if (::cs_open(CS_ARCH_X86, CS_MODE_32, &impl_->handle) != CS_ERR_OK)
    {
        throw std::runtime_error("failed to initialise capstone");
    }

    if (::cs_option(impl_->handle, CS_OPT_DETAIL, CS_OPT_ON) != CS_ERR_OK)
    {
        throw std::runtime_error("failed to set capstone option");
    }
}

Disassembler::~Disassembler()
{
    ::cs_close(&impl_->handle);
}

auto Disassembler::disassemble(std::span<const std::byte> data) const -> std::vector<Instruction>
{
    std::vector<Instruction> instructions{};

    CapstoneDisasembler disassembler{impl_->handle, data};

    for (auto i = 0u; i < disassembler.count; ++i)
    {
        const auto &instruction = disassembler.instruction[i];
        instructions.emplace_back(instruction.mnemonic, instruction.op_str, instruction.size);
    }

    return instructions;
}
}
