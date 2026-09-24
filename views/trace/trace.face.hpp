// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../views.hpp"

namespace SOUND::VIEWS::TRACE {

constexpr STRING::Hot ROWS = "trace.rows";
constexpr STRING::Hot LANES = "trace.lanes";

auto laning() -> Flag;

}  // namespace SOUND::VIEWS::TRACE
