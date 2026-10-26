// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../plugin.hpp"

namespace SOUND::GRAPH {

namespace GET {

auto voices(const String &node) -> Vector<AUDIO::PLUGIN::Voice>;

}  // namespace GET

auto playable(const String &node) -> Flag;

}  // namespace SOUND::GRAPH
