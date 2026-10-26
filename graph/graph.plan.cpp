// SPDX-License-Identifier: AGPL-3.0-or-later
#include <threads.hpp>

#include "graph.plan.hpp"

namespace {
using namespace SOUND;
using std::memory_order_acquire;
using std::memory_order_relaxed;
using std::memory_order_release;

GRAPH::Plan plans[GRAPH::PLAN::SIDES];
THREADS::Shared<Whole> face{0};
THREADS::Shared<Whole> taken{0};
Whole counted = 0;
Whole built = NONE;
Flag owing = true;
Whole reading = 0;

void copied(GRAPH::Plan &plan, const Vector<Whole> &order, Flag whole) {
  const Graph &graph = GRAPH::held();
  const Vector<Flag> &dormant = GRAPH::dormant();
  plan.nodes = graph.nodes;
  plan.wires.clear();
  for (Whole row = 0; row < graph.wires.size() && row < dormant.size(); ++row)
    if (!dormant[row]) plan.wires.push_back(graph.wires[row]);
  plan.stands = GRAPH::GET::seated();
  plan.order = order;
  plan.whole = whole;
  plan.count = ++::counted;
}

}  // namespace

auto SOUND::GRAPH::played() -> const Plan & { return ::plans[::reading]; }

void SOUND::GRAPH::PLAN::owe() { ::owing = true; }

auto SOUND::GRAPH::PLAN::spare() -> Whole {
  if (!::owing && ::built == GRAPH::counted()) return NONE;
  if (::taken.load(memory_order_acquire) != ::counted) return NONE;
  return (::face.load(memory_order_relaxed) + 1) % SIDES;
}

void SOUND::GRAPH::plan(const Vector<Whole> &order, Flag whole) {
  const Whole side = PLAN::spare();
  if (side == NONE) return;
  ::copied(::plans[side], order, whole);
  ::built = GRAPH::counted();
  ::owing = false;
  ::face.store(side, memory_order_release);
}

void SOUND::GRAPH::PLAN::adopt() {
  ::reading = ::face.load(memory_order_acquire);
  ::taken.store(::plans[::reading].count, memory_order_release);
}

auto SOUND::GRAPH::PLAN::adopted() -> Whole {
  return ::taken.load(memory_order_acquire);
}

auto SOUND::GRAPH::PLAN::written() -> Whole { return ::counted; }

auto SOUND::GRAPH::PLAN::side() -> Whole { return ::reading; }

auto SOUND::GRAPH::PLAN::at(const String &name) -> Whole {
  if (name.empty()) return NONE;
  const Vector<Node> &nodes = played().nodes;
  for (Whole node = 0; node < nodes.size(); ++node)
    if (nodes[node].name == name) return node;
  return NONE;
}
