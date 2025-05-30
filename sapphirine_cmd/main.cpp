#include <filesystem>
#include <print>
#include <string>

#include <sapphirine/clean.h>
#include <sapphirine/exception.h>
#include <sapphirine/file.h>
#include <sapphirine/formatter.h>
#include <sapphirine/hello.h>

auto main(int argc, char **argv) -> int
{
    if (argc < 3)
    {
        std::println("Usage: sapphirine_cmd <binary_path> <start_offset>");
        return 1;
    }

    try
    {
        const auto binary_path = std::filesystem::path{argv[1]};
        const auto start_offset = std::stoull(argv[2], nullptr, 16);

        const auto file = sapphirine::File{binary_path, sapphirine::CreationMode::OPEN};

        auto data = file.as_data();

        sapphirine::clean({data.begin() + start_offset, data.end() - start_offset});
    }
    catch (const sapphirine::Exception &e)
    {
        std::println("{}", e);
        return 1;
    }

    return 0;
}
