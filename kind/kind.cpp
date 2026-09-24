// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cartridge/plugin.hpp>
#include <iterator>

#include "kind.doors.hpp"

namespace {

constexpr STRING::Hot NAMES[] = {"audio",   "", "control",
                                 "program", "logic", "data"};

}  // namespace

auto SOUND::KIND::spoken(Whole kind) -> STRING::Hot {
  return kind < std::size(::NAMES) ? ::NAMES[kind] : "unknown";
}

auto SOUND::KIND::meant(const String &token) -> Whole {
  for (Whole kind = 0; kind < std::size(::NAMES); ++kind)
    if (token == ::NAMES[kind]) return kind;
  return NONE;
}

auto SOUND::KIND::carries(Whole kind, Whole family) -> Flag {
  if (kind == NOTES)
    return family == AUDIO::PLUGIN::Event::NOTE_ON ||
           family == AUDIO::PLUGIN::Event::NOTE_OFF ||
           family == AUDIO::PLUGIN::Event::BEND;
  if (kind == PROGRAM) return family == AUDIO::PLUGIN::Event::PROGRAM;
  return kind == CONTROL && family == AUDIO::PLUGIN::Event::CONTROLLER;
}
