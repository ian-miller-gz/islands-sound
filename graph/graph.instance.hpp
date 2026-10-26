// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../plugin.hpp"

namespace SOUND::GRAPH::INSTANCE {

auto parameters(const String &node) -> Whole;

auto described(const String &node, Whole parameter, AUDIO::PLUGIN::Control &out)
  -> Flag;

auto named(const String &node, Whole parameter) -> String;

}  // namespace SOUND::GRAPH::INSTANCE
