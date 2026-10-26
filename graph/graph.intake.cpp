// SPDX-License-Identifier: AGPL-3.0-or-later
#include <threads.hpp>

#include "graph.internal.hpp"

namespace {
using namespace SOUND;
using std::memory_order_acquire;
using std::memory_order_relaxed;
using std::memory_order_release;

THREADS::Shared<Whole> face{0};
THREADS::Shared<Whole> spent{0};
Whole counts[2] = {0, 0};
Whole counted = 0;
Whole reading = 0;
Flag standing = false;

const GRAPH::Intake nothing;

void drained() {
  if (::spent.load(memory_order_acquire) != ::counted) return;
  for (Whole node = 0; node < GRAPH::held().nodes.size(); ++node)
    GRAPH::stand(node).held = {};
}

void published() {
  const Whole live = ::face.load(memory_order_relaxed);
  ::counts[1 - live] = ++::counted;
  for (Whole node = 0; node < GRAPH::held().nodes.size(); ++node)
    GRAPH::stand(node).intakes[1 - live] = GRAPH::stand(node).held;
  ::face.store(1 - live, memory_order_release);
}

void appended(GRAPH::Stand &stand, const GRAPH::Handed &entry) {
  GRAPH::Intake &held = stand.held;
  if (held.size == GRAPH::SLOTS) {
    for (Whole slot = 1; slot < GRAPH::SLOTS; ++slot)
      held.handed[slot - 1] = held.handed[slot];
    --held.size;
  }
  held.handed[held.size++] = entry;
}

}  // namespace

auto SOUND::GRAPH::hand(const String &node, Whole parameter, Float value)
  -> STRING::Hot {
  const STRING::Hot why = vetted(node, parameter);
  if (why[0] != '\0') return why;
  ::drained();
  ::appended(
    stand(at(node)),
    {NONE, {AUDIO::PLUGIN::Event::CONTROLLER, 0, parameter, value}});
  ::published();
  return "";
}

auto SOUND::GRAPH::hand(
  const String &node, Whole out, const AUDIO::PLUGIN::Event &edge) -> Flag {
  if (!rooted(node) || out >= held().nodes[at(node)].outs.size()) return false;
  ::drained();
  ::appended(stand(at(node)), {out, edge});
  ::published();
  return true;
}

void SOUND::GRAPH::hush(const String &node) {
  const Whole stood = at(node);
  if (stood == NONE) return;
  ::drained();
  for (Whole pitch = 0; pitch < PITCHES; ++pitch)
    ::appended(
      stand(stood), {NONE, {AUDIO::PLUGIN::Event::NOTE_OFF, 0, pitch, 0}});
  ::published();
}

auto SOUND::GRAPH::handed(const String &node) -> const Intake & {
  const Whole stood = at(node);
  return stood == NONE ? ::nothing : stand(stood).held;
}

void SOUND::GRAPH::take() {
  const Whole live = ::face.load(memory_order_acquire);
  ::reading = live;
  ::standing = ::counts[live] != ::spent.load(memory_order_relaxed);
  if (::standing) ::spent.store(::counts[live], memory_order_release);
}

auto SOUND::GRAPH::PLAN::taken(Whole node) -> const Intake & {
  const Vector<Stand *> &stands = played().stands;
  if (!::standing || node >= stands.size()) return ::nothing;
  return stands[node]->intakes[::reading];
}
