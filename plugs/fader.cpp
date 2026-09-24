// SPDX-License-Identifier: AGPL-3.0-or-later
#include "fader.hpp"

namespace {
using namespace SOUND;

constexpr Whole SIDES = 2;

auto blend(const FADER::Level &level, Whole channel) -> Float {
  if (channel >= SIDES) return level.gain;
  if (channel == 0)
    return level.gain * (level.pan > FADER::CENTRE ? FADER::UNITY - level.pan
                                                   : FADER::UNITY);
  return level.gain *
         (level.pan < FADER::CENTRE ? FADER::UNITY + level.pan : FADER::UNITY);
}

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  return new FADER::Level{.channels = channels};
}

void destroy(void *instance) { delete static_cast<FADER::Level *>(instance); }

void render(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count) {
  auto &level = *static_cast<FADER::Level *>(instance);
  for (Whole at = 0; at < count; ++at) {
    if (events[at].kind != AUDIO::PLUGIN::Event::CONTROLLER) continue;
    const Float value = FADER::clamped(events[at].index, events[at].value);
    if (events[at].index == FADER::GAIN) level.gain = value;
    if (events[at].index == FADER::PAN) level.pan = value;
  }
  for (Whole channel = 0; channel < level.channels; ++channel) {
    const Float scalar = ::blend(level, channel);
    for (Whole frame = 0; frame < frames; ++frame)
      lanes[channel][frame] *= scalar;
  }
}

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = render,
  .meter = nullptr,
  .destroy = destroy,
  .parameters = FADER::SURFACE::parameters,
  .name = FADER::SURFACE::name,
  .reading = FADER::SURFACE::reading,
  .held = FADER::SURFACE::held,
  .control = FADER::SURFACE::control,
  .ins = {{AUDIO::PLUGIN::Port::AUDIO}, {AUDIO::PLUGIN::Port::CONTROL}},
  .outs = {{AUDIO::PLUGIN::Port::AUDIO}}};

[[maybe_unused]] const Flag offered =
  PLUG::offer({.name = "fader", .surface = &surface});

}  // namespace
