// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <threads.hpp>

#include "render.internal.hpp"

namespace SOUND::RENDER::TAP {
namespace {
using namespace SOUND;
using std::memory_order_acquire;
using std::memory_order_relaxed;
using std::memory_order_release;

constexpr Whole SLOTS = 32;

struct Slot {
  Whole at = 0;
  Whole frames = 0;
  Vector<Vector<Float>> lanes;
};

struct Ring {
  String node;
  Whole in = 0;
  Vector<Slot> slots;
};

struct Side {
  Vector<Ring> rings;
  THREADS::Shared<Whole> written{0};
  THREADS::Shared<Whole> read{0};
};

Side sides[2];
THREADS::Shared<Whole> face{0};
THREADS::Shared<Whole> taken{0};
Whole counts[2] = {0, 0};
Whole counted = 0;
Whole reading = 0;
Vector<RENDER::TAP::Aim> aimed;

auto same(const Vector<RENDER::TAP::Aim> &ins) -> Flag {
  if (ins.size() != aimed.size()) return false;
  for (Whole at = 0; at < ins.size(); ++at)
    if (ins[at].node != aimed[at].node || ins[at].in != aimed[at].in)
      return false;
  return true;
}

auto rung(const RENDER::TAP::Aim &aim) -> Ring {
  Ring ring = {.node = aim.node, .in = aim.in};
  ring.slots.resize(SLOTS);
  for (Slot &slot : ring.slots) {
    slot.lanes.resize(CHANNELS);
    for (Vector<Float> &lane : slot.lanes) lane.assign(RENDER::BLOCK, 0.0f);
  }
  return ring;
}

void emptied(Side &side, Vector<RENDER::TAP::Tapped> &crossed) {
  const Whole read = side.read.load(memory_order_relaxed);
  const Whole wrote = side.written.load(memory_order_acquire);
  for (Whole slot = read; slot < wrote; ++slot)
    for (Ring &ring : side.rings) {
      const Slot &held = ring.slots[slot % SLOTS];
      RENDER::TAP::Tapped block = {.at = held.at, .in = ring.in};
      block.lanes.resize(held.lanes.size());
      for (Whole lane = 0; lane < held.lanes.size(); ++lane)
        block.lanes[lane].assign(
          held.lanes[lane].begin(), held.lanes[lane].begin() + held.frames);
      crossed.push_back(block);
    }
  side.read.store(wrote, memory_order_release);
}

}  // namespace
}  // namespace SOUND::RENDER::TAP

void SOUND::RENDER::TAP::aim(const Vector<Aim> &ins) {
  if (same(ins)) return;
  if (taken.load(memory_order_acquire) != counted) return;
  const Whole writing = 1 - face.load(memory_order_relaxed);
  Side &side = sides[writing];
  side.rings.clear();
  for (const Aim &in : ins) side.rings.push_back(rung(in));
  side.written.store(0, memory_order_relaxed);
  side.read.store(0, memory_order_relaxed);
  counts[writing] = ++counted;
  face.store(writing, memory_order_release);
  aimed = ins;
}

auto SOUND::RENDER::TAP::drained() -> Vector<Tapped> {
  Vector<Tapped> crossed;
  emptied(sides[0], crossed);
  emptied(sides[1], crossed);
  return crossed;
}

void SOUND::RENDER::TAP::adopt() {
  const Whole live = face.load(memory_order_acquire);
  reading = live;
  taken.store(counts[live], memory_order_release);
}

void SOUND::RENDER::TAP::fed(Whole start, Whole frames) {
  Side &side = sides[reading];
  if (side.rings.empty()) return;
  const Whole wrote = side.written.load(memory_order_relaxed);
  if (wrote - side.read.load(memory_order_acquire) == SLOTS) return;
  for (Ring &ring : side.rings) {
    Slot &slot = ring.slots[wrote % SLOTS];
    const Vector<Vector<AUDIO::PLUGIN::Sample>> &lanes =
      landed(ring.node, ring.in);
    slot.at = start;
    slot.frames = std::min<Whole>(frames, BLOCK);
    for (Whole lane = 0; lane < slot.lanes.size(); ++lane)
      for (Whole frame = 0; frame < slot.frames; ++frame)
        slot.lanes[lane][frame] =
          lane < lanes.size() ? lanes[lane][frame] : 0.0f;
  }
  side.written.store(wrote + 1, memory_order_release);
}
