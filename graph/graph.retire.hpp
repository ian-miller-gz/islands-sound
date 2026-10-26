// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../plugin.hpp"

namespace SOUND::GRAPH {

auto retired() -> Whole;

void reap(Whole adopted);

namespace SIGNAL {

auto counted() -> Whole;

void clean();

auto touched() -> Flag;

}  // namespace SIGNAL

}  // namespace SOUND::GRAPH
