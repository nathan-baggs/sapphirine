#include "sapphirine/clean.h"

#include <cstddef>
#include <span>

#include "sapphirine/log.h"

namespace sapphirine
{

auto clean(std::span<const std::byte> data) -> void
{
    log::info("Cleaning {} bytes of data", data.size_bytes());
}

}
