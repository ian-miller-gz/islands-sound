// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::TRACE {

constexpr STRING::Hot ROWS = "trace.rows";
constexpr STRING::Hot LANES = "trace.lanes";
constexpr STRING::Hot CLIPS = "trace.clips";

auto faced() -> Whole;

namespace GET {

auto list() -> STRING::Hot;

}  // namespace GET

}  // namespace SOUND::VIEWS::TRACE
