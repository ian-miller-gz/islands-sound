// SPDX-License-Identifier: AGPL-3.0-or-later
#include <threads.hpp>

#include "graph.internal.hpp"

namespace {
using namespace SOUND;
using std::memory_order_acquire;
using std::memory_order_relaxed;
using std::memory_order_release;

THREADS::Shared<Whole> face{0};
THREADS::Shared<Whole> taken{0};
Whole counts[2] = {0, 0};
Whole counted = 0;
Whole writing = 0;
Whole reading = 0;

}  // namespace

auto SOUND::GRAPH::written(Stand &stand) -> Tape & {
  return stand.tapes[::writing];
}

auto SOUND::GRAPH::walked(const Stand &stand) -> const Tape & {
  return stand.tapes[::reading];
}

auto SOUND::GRAPH::spare() -> Flag {
  if (::taken.load(memory_order_acquire) != ::counted) return false;
  ::writing = 1 - ::face.load(memory_order_relaxed);
  return true;
}

void SOUND::GRAPH::publish() {
  ::counts[::writing] = ++::counted;
  ::face.store(::writing, memory_order_release);
}

auto SOUND::GRAPH::adopt() -> Flag {
  const Whole live = ::face.load(memory_order_acquire);
  const Flag fresh = ::counts[live] != ::taken.load(memory_order_relaxed);
  ::reading = live;
  ::taken.store(::counts[live], memory_order_release);
  return fresh;
}
