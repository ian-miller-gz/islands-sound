// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cartridge/plugin.hpp>
#include <ranges>

#include "kind.doors.hpp"
#include "kind.named.hpp"

namespace {
using namespace SOUND::KIND;

constexpr auto exact(Whole kind) -> Flag { return meant(spoken(kind)) == kind; }

static_assert(named(NAMES));
static_assert(
  std::ranges::all_of(std::views::iota(Whole{}, std::size(NAMES)), exact));

}  // namespace

auto SOUND::KIND::carries(Whole kind, Whole family) -> Flag {
  if (kind == NOTES)
    return family == AUDIO::PLUGIN::Event::NOTE_ON ||
           family == AUDIO::PLUGIN::Event::NOTE_OFF ||
           family == AUDIO::PLUGIN::Event::BEND;
  if (kind == PROGRAM) return family == AUDIO::PLUGIN::Event::PROGRAM;
  return kind == CONTROL && family == AUDIO::PLUGIN::Event::CONTROLLER;
}
