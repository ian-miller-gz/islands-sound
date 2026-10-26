// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "graph.hpp"

namespace SOUND::GRAPH::VETTED {

constexpr STRING::Hot CLEAN = "";
constexpr STRING::Hot NODE = "no such node";
constexpr STRING::Hot PLUGIN = "node seats no plugin";
constexpr STRING::Hot PARAMETER = "no such parameter";

}  // namespace SOUND::GRAPH::VETTED

namespace SOUND::GRAPH {

auto vetted(const String &node, Whole parameter) -> STRING::Hot;

}  // namespace SOUND::GRAPH
