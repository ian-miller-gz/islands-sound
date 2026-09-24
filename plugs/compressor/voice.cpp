// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  auto *press = new COMPRESSOR::Press{.rate = rate, .channels = channels};
  COMPRESSOR::bake(*press);
  for (Whole id = 0; id < COMPRESSOR::PARAMETERS; ++id)
    press->rows[id] = COMPRESSOR::resting(id);
  COMPRESSOR::settle(*press);
  return press;
}

auto meter(void *instance) -> Float {
  auto &press = *static_cast<COMPRESSOR::Press *>(instance);
  return press.peak[press.face.load(std::memory_order_acquire)];
}

void destroy(void *instance) {
  delete static_cast<COMPRESSOR::Press *>(instance);
}

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = COMPRESSOR::render,
  .meter = meter,
  .destroy = destroy,
  .parameters = COMPRESSOR::SURFACE::parameters,
  .name = COMPRESSOR::SURFACE::name,
  .reading = COMPRESSOR::SURFACE::reading,
  .held = COMPRESSOR::SURFACE::held,
  .control = COMPRESSOR::SURFACE::control,
  .ins = {{AUDIO::PLUGIN::Port::AUDIO}, {AUDIO::PLUGIN::Port::CONTROL}},
  .outs = {{AUDIO::PLUGIN::Port::AUDIO}}};

[[maybe_unused]] const Flag offered =
  PLUG::offer({.name = "compressor", .surface = &surface});

}  // namespace

void SOUND::COMPRESSOR::apply(Press &press, const AUDIO::PLUGIN::Event &event) {
  if (event.kind != AUDIO::PLUGIN::Event::CONTROLLER) return;
  if (event.index >= PARAMETERS) return;
  press.rows[event.index] = clamped(event.index, event.value);
  settle(press);
}
