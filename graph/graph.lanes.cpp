// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../kind.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot RECORDING = "lane is sending and recording";

auto seat(const String &root, Whole out) -> Node * {
  if (!GRAPH::rooted(root)) return nullptr;
  Node &held = GRAPH::standing().nodes[GRAPH::at(root)];
  return out < held.outs.size() ? &held : nullptr;
}

auto before(const Port &one, const Port &two) -> Flag {
  return one.lane < two.lane;
}

auto sending(const String &root, Whole out) -> Flag {
  const String own = GRAPH::recorder(root);
  for (const Wire &wire : GRAPH::held().wires)
    if (wire.from == root && wire.out == out && wire.to != own) return true;
  return false;
}

void unfed(const String &root, Whole in) {
  Graph &wiring = GRAPH::standing();
  std::erase_if(wiring.wires, [&](const Wire &wire) {
    return wire.to == root && wire.in == in;
  });
  std::erase_if(wiring.slack, [&](const Wire &wire) {
    return wire.to == root && wire.in == in;
  });
  for (Wire &wire : wiring.wires)
    if (wire.to == root && wire.in > in) --wire.in;
  for (Wire &wire : wiring.slack)
    if (wire.to == root && wire.in > in) --wire.in;
}

}  // namespace

auto SOUND::GRAPH::admit(const String &root, Whole out, Flag on) -> Flag {
  Node *held = ::seat(root, out);
  if (held == nullptr) return false;
  const Whole in = admitted(root, out);
  if (on == (in != NONE)) return true;
  if (!on) {
    ::unfed(root, in);
    held->ins.erase(held->ins.begin() + static_cast<Integer>(in));
  } else {
    Port port = mirrored(held->outs[out]);
    port.lane = out;
    const auto seat =
      std::upper_bound(held->ins.begin(), held->ins.end(), port, ::before);
    const Whole at = Whole(seat - held->ins.begin());
    held->ins.insert(seat, port);
    for (Wire &wire : standing().wires)
      if (wire.to == root && wire.in >= at) ++wire.in;
    for (Wire &wire : standing().slack)
      if (wire.to == root && wire.in >= at) ++wire.in;
  }
  stir();
  return true;
}

auto SOUND::GRAPH::admitted(const String &root, Whole out) -> Whole {
  const Node *held = ::seat(root, out);
  if (held == nullptr) return NONE;
  for (Whole in = 0; in < held->ins.size(); ++in)
    if (held->ins[in].lane == out) return in;
  return NONE;
}

auto SOUND::GRAPH::bypass(const String &root, Whole out, Flag on)
  -> STRING::Hot {
  Node *held = ::seat(root, out);
  if (held == nullptr) return "no such lane";
  if (!on && ::sending(root, out) && joined(recorder(root), Side::IN, out) != 0)
    return ::RECORDING;
  held->outs[out].bypassed = on;
  return "";
}

auto SOUND::GRAPH::bypassed(const String &root, Whole out) -> Flag {
  const Node *held = ::seat(root, out);
  return held != nullptr && held->outs[out].bypassed;
}
