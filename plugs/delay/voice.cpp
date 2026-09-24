// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  auto *trail = new DELAY::Trail{.rate = rate, .channels = channels};
  DELAY::stretch(*trail);
  for (Whole id = 0; id < DELAY::PARAMETERS; ++id)
    trail->rows[id] = DELAY::resting(id);
  DELAY::settle(*trail);
  return trail;
}

auto meter(void *instance) -> Float {
  auto &trail = *static_cast<DELAY::Trail *>(instance);
  return trail.peak[trail.face.load(std::memory_order_acquire)];
}

void destroy(void *instance) { delete static_cast<DELAY::Trail *>(instance); }

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = DELAY::render,
  .meter = meter,
  .destroy = destroy,
  .parameters = DELAY::SURFACE::parameters,
  .name = DELAY::SURFACE::name,
  .reading = DELAY::SURFACE::reading,
  .held = DELAY::SURFACE::held,
  .control = DELAY::SURFACE::control,
  .ins = {{AUDIO::PLUGIN::Port::AUDIO}, {AUDIO::PLUGIN::Port::CONTROL}},
  .outs = {{AUDIO::PLUGIN::Port::AUDIO}}};

[[maybe_unused]] const Flag offered =
  PLUG::offer({.name = "delay", .surface = &surface});

}  // namespace

void SOUND::DELAY::apply(Trail &trail, const AUDIO::PLUGIN::Event &event) {
  if (event.kind != AUDIO::PLUGIN::Event::CONTROLLER) return;
  if (event.index >= PARAMETERS) return;
  trail.rows[event.index] = clamped(event.index, event.value);
  settle(trail);
}
