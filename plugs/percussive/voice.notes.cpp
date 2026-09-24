// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr uint32_t SEED = 0x9e3779b9u;
constexpr uint32_t STRIDE = 0x85ebca6bu;

auto struck(Whole pitch) -> Whole {
  for (Whole slot = 0; slot < PERCUSSIVE::SLOTS; ++slot)
    if (PERCUSSIVE::recipe(slot).pitch == pitch) return slot;
  return PERCUSSIVE::SLOTS;
}

void hit(PERCUSSIVE::Machine &machine, Whole slot, Float velocity) {
  const PERCUSSIVE::Recipe &made = PERCUSSIVE::recipe(slot);
  PERCUSSIVE::Strike &strike = machine.strikes[slot];
  strike.phase = 0;
  strike.step = made.hertz * machine.wheel;
  strike.rest = made.floor * machine.wheel;
  strike.bend =
    (strike.step - strike.rest) * PERCUSSIVE::delta(made.bend, machine.rate);
  strike.level = 1;
  strike.drop = PERCUSSIVE::delta(machine.decays[slot], machine.rate);
  strike.noise = SEED + uint32_t(slot) * STRIDE;
  strike.held = 0;
  strike.velocity = velocity < 0 ? 0 : velocity > 1 ? 1 : velocity;
}

}  // namespace

void SOUND::PERCUSSIVE::steer(Machine &machine, Whole id, Float value) {
  const Float held = clamped(id, value);
  if (id == GAIN) {
    machine.gain = held;
    return;
  }
  if (id == CHOKE) {
    machine.choke = held;
    return;
  }
  const Whole place = slot(id);
  if (place >= SLOTS) return;
  if (lane(id) == LEVEL)
    machine.levels[place] = held;
  else
    machine.decays[place] = held;
}

void SOUND::PERCUSSIVE::apply(
  Machine &machine, const AUDIO::PLUGIN::Event &event) {
  if (event.kind == AUDIO::PLUGIN::Event::CONTROLLER) {
    steer(machine, event.index, event.value);
    return;
  }
  if (event.kind != AUDIO::PLUGIN::Event::NOTE_ON || event.value <= 0) return;
  const Whole place = ::struck(event.index);
  if (place >= SLOTS) return;
  ::hit(machine, place, event.value);
  if (place == CLOSED && machine.choke != 0) machine.strikes[OPEN].level = 0;
}
