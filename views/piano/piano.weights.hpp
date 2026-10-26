// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>

namespace SOUND::VIEWS::ROLL::WEIGHTS {

constexpr Float LINE = 0.13f, STEM = 0.12f, BEAM = 0.5f, GAP = 0.25f;
constexpr Float LEDGER = 0.16f, REACH = 0.4f, BAR = 0.16f;

struct Across {
  Float spaces = 0.0f;
};

struct Down {
  Float spaces = 0.0f;
};

auto drawn(Float spaces) -> Float;
auto drawn(Across weight) -> Float;
auto drawn(Down weight) -> Float;

auto seated(Float y) -> Float;

}  // namespace SOUND::VIEWS::ROLL::WEIGHTS
