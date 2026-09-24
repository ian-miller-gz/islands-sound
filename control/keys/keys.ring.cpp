// SPDX-License-Identifier: AGPL-3.0-or-later
#include "keys.hpp"

using std::memory_order_acquire;
using std::memory_order_relaxed;
using std::memory_order_release;

auto SOUND::KEYS::Ring::push(const AUDIO::PLUGIN::Event &edge) -> Flag {
  const Whole slot = tail.load(memory_order_relaxed);
  const Whole next = (slot + 1) % SIZE;
  if (next == head.load(memory_order_acquire)) return false;
  slots[slot] = edge;
  tail.store(next, memory_order_release);
  return true;
}

auto SOUND::KEYS::Ring::pop(AUDIO::PLUGIN::Event &edge) -> Flag {
  const Whole slot = head.load(memory_order_relaxed);
  if (slot == tail.load(memory_order_acquire)) return false;
  edge = slots[slot];
  head.store((slot + 1) % SIZE, memory_order_release);
  return true;
}
