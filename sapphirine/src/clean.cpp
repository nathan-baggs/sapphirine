#include "sapphirine/clean.h"

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <iterator>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <vector>

#include "sapphirine/disassembler.h"
#include "sapphirine/exception.h"
#include "sapphirine/formatter.h"
#include "sapphirine/instruction.h"
#include "sapphirine/log.h"

namespace
{

static constexpr auto max_instruction_size = 15llu;

auto follow_jumps(
    const sapphirine::Disassembler &disassembler,
    std::span<const std::byte> data,
    std::size_t virtual_address,
    std::size_t start) -> sapphirine::JumpChain
{
    auto result = sapphirine::JumpChain{start, 0, 0};
    auto cf = false;

    while (!data.empty())
    {
        const auto instructions = disassembler.disassemble(data.first(std::min(data.size(), max_instruction_size)));
        if (instructions.empty())
        {
            sapphirine::log::info("no instructions found in the data");
            break;
        }

        const auto instruction = instructions.front();

        if (!instruction.mnemonic().starts_with("j"))
        {
            sapphirine::log::info("not a jump instruction: {} [{:#x}]", instruction, virtual_address + result.consumed);

            if (instruction.mnemonic() == "stc")
            {
                sapphirine::log::info("found stc instruction, skipping [{:#x}]", virtual_address + result.consumed);
                data = data.subspan(instruction.size());
                result.consumed += instruction.size();
                cf = true;
                continue;
            }
            else
            {
                break;
            }
        }

        sapphirine::log::info("following jump instruction: {} [{:#x}]", instruction, virtual_address + result.consumed);

        if (instruction.mnemonic() == "jae" && cf)
        {
            sapphirine::log::info(
                "found jae instruction with carry flag set, skipping [{:#x}]", virtual_address + result.consumed);
            data = data.subspan(instruction.size());
            result.consumed += instruction.size();
            continue;
        }

        const auto operand_offset = instruction.operands().starts_with("0x") ? 2u : 0u;

        auto offset = std::uint32_t{0};
        if (const auto [ptr, ec] = std::from_chars(
                instruction.operands().data() + operand_offset,
                instruction.operands().data() + operand_offset + instruction.operands().size(),
                offset,
                16);
            ec != std::errc{})
        {
            throw sapphirine::Exception(
                "failed to parse jump offset: {} [{:#x}]", instruction, virtual_address + result.consumed);
        }

        if (static_cast<std::int32_t>(offset) < 0)
        {
            sapphirine::log::info(
                "jump offset is negative, ending [{:#x}]", virtual_address + result.consumed + offset);
            break;
        }

        ++result.jump_count;
        result.consumed += offset;

        data = data.subspan(offset);
    }

    return result;
}

auto disassemble_next_instruction(const sapphirine::Disassembler &disassembler, std::span<const std::byte> data)
    -> std::optional<sapphirine::Instruction>
{
    try
    {
        const auto instructions = disassembler.disassemble(data.first(max_instruction_size));

        return instructions.empty() ? std::nullopt : std::make_optional(instructions.front());
    }
    catch (const sapphirine::Exception &)
    {
        return std::nullopt;
    }
}
}

namespace sapphirine
{

auto JumpChain::to_string() const -> std::string
{
    return std::format("start: {}, consumed: {}, jump_count: {}", start, consumed, jump_count);
}

auto clean(std::span<const std::byte> data, std::size_t virtual_address) -> std::vector<JumpChain>
{
    auto jump_chains = std::vector<JumpChain>{};
    auto offset = std::size_t{0};

    log::info("Cleaning {} bytes of data [{:#x}]", data.size_bytes(), virtual_address + offset);

    const auto disassembler = Disassembler{};

    while (!data.empty())
    {
        const auto attempt_instruction = disassemble_next_instruction(disassembler, data);
        if (!attempt_instruction.has_value())
        {
            log::info("no more instructions found, stopping [{:#x}]", virtual_address + offset);
            break;
        }

        const auto instruction = attempt_instruction.value();

        if (instruction.mnemonic().starts_with("j"))
        {
            log::info("found jump instruction: {} [{:#x}]", instruction, virtual_address + offset);

            const auto jump_chain = follow_jumps(disassembler, data, virtual_address + offset, offset);

            log::info("jump chain: {} [{:#x}]", jump_chain, virtual_address + offset);

            if (jump_chain.jump_count >= 3)
            {
                log::info("found jump chain [{:#x}]", virtual_address + offset);
                jump_chains.push_back(jump_chain);
                data = data.subspan(jump_chain.consumed);
                offset += jump_chain.consumed;
            }
            else
            {
                log::info(
                    "not a jump chain, only {} jumps found [{:#x}]", jump_chain.jump_count, virtual_address + offset);
                data = data.subspan(instruction.size());
                offset += instruction.size();
            }
        }
        else
        {
            log::info("found non-jump instruction: {} [{:#x}]", instruction, virtual_address + offset);
            if (instruction.size() >= data.size())
            {
                break;
            }
            data = data.subspan(instruction.size());
            offset += instruction.size();
        }
    }

    return jump_chains;
}

}
