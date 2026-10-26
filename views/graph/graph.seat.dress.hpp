// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../graph.hpp"

namespace SOUND::VIEWS::WIRED {

auto dressed(const Node &node) -> String;

auto surfaced(const Node &node) -> Flag;

auto dotted(const Node &node) -> Flag;

auto subtitled(const Node &node) -> String;

}  // namespace SOUND::VIEWS::WIRED
