// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <algorithm>

#include <common.hpp>

namespace SOUND::KIND {

template <Whole size>
constexpr auto named(const STRING::Hot (&names)[size]) -> Flag {
  return std::ranges::none_of(
    names, [](STRING::Hot name) { return !name || !*name; });
}

}  // namespace SOUND::KIND
