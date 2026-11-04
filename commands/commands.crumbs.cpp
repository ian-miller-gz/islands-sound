// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <iterator>

#include "../views.hpp"
#include "commands.offer.hpp"

namespace {
using namespace SOUND;
namespace SPLIT = SOUND::VIEWS::SPLIT;

constexpr STRING::Hot COMMAND = "crumbs";
constexpr STRING::Hot HELP = "";
constexpr STRING::Hot LINE = "crumbs {} {}";
constexpr STRING::Hot BARE = "-";

void crumbs(SHELL::Session &session) {
  for (Whole region = 0; region < std::size(SPLIT::REGIONS); ++region) {
    const String said = VIEWS::CRUMBS::read(region);
    session.print(std::format(
      ::LINE, SPLIT::REGIONS[region], said.empty() ? String(::BARE) : said));
  }
}

[[maybe_unused]] const Flag offered =
  COMMANDS::offer({{::COMMAND, ::HELP, ::crumbs}});

}  // namespace
