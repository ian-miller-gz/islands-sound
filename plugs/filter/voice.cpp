// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  auto *sieve = new FILTER::Sieve{.rate = rate, .channels = channels};
  sieve->lanes.assign(channels, FILTER::Lane{});
  FILTER::bake(*sieve);
  for (Whole id = 0; id < FILTER::PARAMETERS; ++id)
    sieve->rows[id] = FILTER::resting(id);
  FILTER::settle(*sieve);
  return sieve;
}

auto meter(void *instance) -> Float {
  auto &sieve = *static_cast<FILTER::Sieve *>(instance);
  return sieve.peak[sieve.face.load(std::memory_order_acquire)];
}

void destroy(void *instance) { delete static_cast<FILTER::Sieve *>(instance); }

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = FILTER::render,
  .meter = meter,
  .destroy = destroy,
  .parameters = FILTER::SURFACE::parameters,
  .name = FILTER::SURFACE::name,
  .reading = FILTER::SURFACE::reading,
  .held = FILTER::SURFACE::held,
  .control = FILTER::SURFACE::control,
  .ins = {{AUDIO::PLUGIN::Port::AUDIO}, {AUDIO::PLUGIN::Port::CONTROL}},
  .outs = {{AUDIO::PLUGIN::Port::AUDIO}}};

[[maybe_unused]] const Flag offered =
  PLUG::offer({.name = "filter", .surface = &surface});

}  // namespace

void SOUND::FILTER::apply(Sieve &sieve, const AUDIO::PLUGIN::Event &event) {
  if (event.kind != AUDIO::PLUGIN::Event::CONTROLLER) return;
  if (event.index >= PARAMETERS) return;
  sieve.rows[event.index] = clamped(event.index, event.value);
  settle(sieve);
}
