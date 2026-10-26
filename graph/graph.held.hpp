// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "graph.hpp"

namespace SOUND::GRAPH {

auto hold(
  const String &plugin, const String &name, const Vector<Port> &ins,
  const Vector<Port> &outs) -> String;

auto missing(const String &node) -> Flag;

}  // namespace SOUND::GRAPH
