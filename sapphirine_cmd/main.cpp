#include <cstddef>
#include <filesystem>
#include <print>
#include <string>
#include <vector>

#include <sapphirine/clean.h>
#include <sapphirine/exception.h>
#include <sapphirine/file.h>
#include <sapphirine/formatter.h>
#include <sapphirine/log.h>

auto main(int argc, char **argv) -> int
{
    if (argc < 5)
    {
        std::println("Usage: sapphirine_cmd <binary_path> <start_offset> <virtual_address> <output_path>");
        return 1;
    }

    try
    {
        const auto binary_path = std::filesystem::path{argv[1]};
        const auto start_offset = std::stoull(argv[2], nullptr, 16);
        const auto virtual_address = std::stoull(argv[3], nullptr, 16);
        const auto output_path = std::filesystem::path{argv[4]};

        const auto file = sapphirine::File{binary_path, sapphirine::CreationMode::OPEN};

        const auto data = file.as_data();

        const auto jump_chains =
            sapphirine::clean({data.begin() + start_offset, data.end() - start_offset}, virtual_address);

        auto patched_file = std::vector<std::byte>{data.begin(), data.end()};

        for (const auto &jump_chain : jump_chains)
        {
            sapphirine::log::info(
                "patching jump chain: {} at [{:#x}]",
                jump_chain.to_string(),
                virtual_address + start_offset + jump_chain.start);
            const auto start = start_offset + jump_chain.start;
            const auto end = start + jump_chain.consumed;

            std::fill(patched_file.begin() + start, patched_file.begin() + end, std::byte{0x90});
        }

        sapphirine::File output_file{
            output_path, sapphirine::CreationMode::CREATE, static_cast<std::uint32_t>(patched_file.size())};
        output_file.write(patched_file);

        sapphirine::log::info("done");
    }
    catch (const sapphirine::Exception &e)
    {
        std::println("{}", e);
        return 1;
    }

    return 0;
}
