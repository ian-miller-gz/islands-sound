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
  for (GRAPH::Stand &stand : GRAPH::stands()) stand.held = {};
}

void published() {
  const Whole live = ::face.load(memory_order_relaxed);
  ::counts[1 - live] = ++::counted;
  for (GRAPH::Stand &stand : GRAPH::stands())
    stand.intakes[1 - live] = stand.held;
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
    stands()[at(node)],
    {NONE, {AUDIO::PLUGIN::Event::CONTROLLER, 0, parameter, value}});
  ::published();
  return "";
}

auto SOUND::GRAPH::hand(const String &node, const AUDIO::PLUGIN::Event &edge)
  -> Flag {
  if (!rooted(node)) return false;
  const Whole out = worn(node, KIND::NOTES);
  return out != NONE && hand(node, out, edge);
}

auto SOUND::GRAPH::hand(
  const String &node, Whole out, const AUDIO::PLUGIN::Event &edge) -> Flag {
  if (!rooted(node) || out >= held().nodes[at(node)].outs.size()) return false;
  ::drained();
  ::appended(stands()[at(node)], {out, edge});
  ::published();
  return true;
}

void SOUND::GRAPH::hush(const String &node) {
  const Whole stood = at(node);
  if (stood == NONE) return;
  ::drained();
  for (Whole pitch = 0; pitch < PITCHES; ++pitch)
    ::appended(
      stands()[stood], {NONE, {AUDIO::PLUGIN::Event::NOTE_OFF, 0, pitch, 0}});
  ::published();
}

auto SOUND::GRAPH::handed(const String &node) -> const Intake & {
  const Whole stood = at(node);
  return stood == NONE ? ::nothing : stands()[stood].held;
}

void SOUND::GRAPH::take() {
  const Whole live = ::face.load(memory_order_acquire);
  ::reading = live;
  ::standing = ::counts[live] != ::spent.load(memory_order_relaxed);
  if (::standing) ::spent.store(::counts[live], memory_order_release);
}

auto SOUND::GRAPH::taken(const String &node) -> const Intake & {
  const Whole stood = at(node);
  if (!::standing || stood == NONE) return ::nothing;
  return stands()[stood].intakes[::reading];
}
