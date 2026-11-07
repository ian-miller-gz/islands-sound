// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <iterator>

#include "kind.hpp"
#include "../score.hpp"

namespace SOUND::KIND {

constexpr STRING::Hot NAMES[] = {"audio",   "notes", "control",
                                 "program", "logic", "data"};

constexpr auto spoken(Whole kind) -> STRING::Hot {
  return kind < std::size(NAMES) ? NAMES[kind] : "unknown";
}

constexpr auto meant(const String &token) -> Whole {
  for (Whole kind = 0; kind < std::size(NAMES); ++kind)
    if (token == NAMES[kind]) return kind;
  return NONE;
}

auto carries(Whole kind, Whole family) -> Flag;

}  // namespace SOUND::KIND
