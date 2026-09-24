// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "kind.hpp"
#include "../score.hpp"

namespace SOUND::KIND {

auto spoken(Whole kind) -> STRING::Hot;

auto meant(const String &token) -> Whole;

auto carries(Whole kind, Whole family) -> Flag;

}  // namespace SOUND::KIND
