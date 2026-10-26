// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../score.hpp"

namespace SOUND::RENDER {

auto governs(Whole track, Whole lane) -> String;

auto wired(const String &root, Whole out, const String &node) -> Flag;

}  // namespace SOUND::RENDER
