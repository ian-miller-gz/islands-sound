// SPDX-License-Identifier: AGPL-3.0-or-later
#include <filesystem>
#include <format>

#include <common/platform/paths.hpp>

#include "../master.hpp"
#include "inventory.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LEAF = "/islands/takes";
constexpr STRING::Hot UNNAMED = "unnamed";

auto home() -> String {
  const String data = COMMON::PLATFORM::data();
  std::error_code slip;
  const String root =
    data.empty() ? std::filesystem::temp_directory_path(slip).string() : data;
  return root + LEAF;
}

}  // namespace

auto SOUND::INVENTORY::house(const String &session) -> Whole {
  Inventory &pool = standing();
  const String owner = session.empty() ? String(::UNNAMED) : session;
  Whole housed = 0;
  for (Whole index = 0; index < pool.stocks.size(); ++index) {
    Take &take = pool.stocks[index].take;
    if (pool.stocks[index].kind != KIND::AUDIO) continue;
    if (!take.path.empty() || take.lanes.empty() || take.lanes[0].empty())
      continue;
    const String directory = ::home();
    std::error_code slip;
    std::filesystem::create_directories(directory, slip);
    const String path = std::format("{}/{}-{}.wav", directory, owner, index);
    if (!MASTER::WAVE::write(path, RATE, take.lanes)) continue;
    take.path = path;
    ++housed;
  }
  return housed;
}
