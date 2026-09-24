// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float TAU = 6.2831853f;
constexpr Float WHEEL = 4294967296.0f;

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  auto *machine = new PERCUSSIVE::Machine{.rate = rate, .channels = channels};
  machine->wave.resize(PERCUSSIVE::TABLE);
  for (Whole place = 0; place < PERCUSSIVE::TABLE; ++place)
    machine->wave[place] =
      std::sin(TAU * Float(place) / Float(PERCUSSIVE::TABLE));
  machine->wheel = WHEEL / Float(rate);
  for (Whole id = 0; id < PERCUSSIVE::PARAMETERS; ++id)
    PERCUSSIVE::apply(
      *machine,
      {AUDIO::PLUGIN::Event::CONTROLLER, 0, id, PERCUSSIVE::resting(id)});
  return machine;
}

auto meter(void *instance) -> Float {
  auto &machine = *static_cast<PERCUSSIVE::Machine *>(instance);
  return machine.level[machine.face.load(std::memory_order_acquire)];
}

void destroy(void *instance) {
  delete static_cast<PERCUSSIVE::Machine *>(instance);
}

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = PERCUSSIVE::render,
  .meter = meter,
  .destroy = destroy,
  .parameters = PERCUSSIVE::SURFACE::parameters,
  .name = PERCUSSIVE::SURFACE::name,
  .reading = PERCUSSIVE::SURFACE::reading,
  .held = PERCUSSIVE::SURFACE::held,
  .control = PERCUSSIVE::SURFACE::control,
  .ins = {{AUDIO::PLUGIN::Port::NOTES}, {AUDIO::PLUGIN::Port::CONTROL}},
  .outs = {{AUDIO::PLUGIN::Port::AUDIO}}};

[[maybe_unused]] const Flag offered =
  PLUG::offer({.name = "percussive", .surface = &surface});

}  // namespace

auto SOUND::PERCUSSIVE::delta(Float span, Whole rate) -> Float {
  const Float frames = span * Float(rate);
  return frames <= 1.0f ? 1.0f : 1.0f / frames;
}
