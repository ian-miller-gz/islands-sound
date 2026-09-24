// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float TAU = 6.2831853f;
constexpr Whole NOTES = 128;
constexpr Float OCTAVE = 12;
constexpr Float CENT = 1200;
constexpr Float WHEEL = 4294967296.0f;

auto increment(Whole pitch, Whole rate) -> uint32_t {
  const Float hertz = 440.0f * std::pow(2.0f, (Float(pitch) - 69.0f) / OCTAVE);
  const Float turns = hertz / Float(rate);
  return static_cast<uint32_t>(turns * WHEEL);
}

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  auto *synth = new SUBTRACTIVE::Synth{.rate = rate, .channels = channels};
  synth->steps.resize(NOTES);
  for (Whole pitch = 0; pitch < NOTES; ++pitch)
    synth->steps[pitch] = increment(pitch, rate);
  synth->ratios.resize(SUBTRACTIVE::CENTS + 1);
  for (Whole cent = 0; cent <= SUBTRACTIVE::CENTS; ++cent)
    synth->ratios[cent] = std::pow(2.0f, Float(cent) / CENT);
  synth->turn = TAU / Float(rate);
  for (Whole id = 0; id < SUBTRACTIVE::PARAMETERS; ++id)
    SUBTRACTIVE::apply(
      *synth,
      {AUDIO::PLUGIN::Event::CONTROLLER, 0, id, SUBTRACTIVE::resting(id)});
  return synth;
}

auto meter(void *instance) -> Float {
  auto &synth = *static_cast<SUBTRACTIVE::Synth *>(instance);
  return synth.level[synth.face.load(std::memory_order_acquire)];
}

void destroy(void *instance) {
  delete static_cast<SUBTRACTIVE::Synth *>(instance);
}

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = SUBTRACTIVE::render,
  .meter = meter,
  .destroy = destroy,
  .parameters = SUBTRACTIVE::SURFACE::parameters,
  .name = SUBTRACTIVE::SURFACE::name,
  .reading = SUBTRACTIVE::SURFACE::reading,
  .held = SUBTRACTIVE::SURFACE::held,
  .control = SUBTRACTIVE::SURFACE::control,
  .ins = {{AUDIO::PLUGIN::Port::NOTES}, {AUDIO::PLUGIN::Port::CONTROL}},
  .outs = {{AUDIO::PLUGIN::Port::AUDIO}}};

[[maybe_unused]] const Flag offered =
  PLUG::offer({.name = "subtractive", .surface = &surface});

}  // namespace

auto SOUND::SUBTRACTIVE::delta(Float span, Whole rate) -> Float {
  const Float frames = span * Float(rate);
  return frames <= 1.0f ? 1.0f : 1.0f / frames;
}

void SOUND::SUBTRACTIVE::shape(Envelope &envelope, Whole rate) {
  envelope.rise = delta(envelope.attack, rate);
  envelope.fall = delta(envelope.decay, rate) * (1.0f - envelope.sustain);
  envelope.drop = delta(envelope.release, rate);
}
