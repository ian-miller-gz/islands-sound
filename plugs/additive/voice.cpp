// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float TAU = 6.2831853f;
constexpr Whole NOTES = 128;

auto increment(Whole pitch, Whole rate) -> uint32_t {
  const Float hertz = 440.0f * std::pow(2.0f, (Float(pitch) - 69.0f) / 12.0f);
  const Float turns = hertz / Float(rate);
  return static_cast<uint32_t>(turns * 4294967296.0f);
}

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  auto *synth = new ADDITIVE::Synth{.rate = rate, .channels = channels};
  synth->table.resize(ADDITIVE::TABLE);
  for (Whole slot = 0; slot < ADDITIVE::TABLE; ++slot)
    synth->table[slot] = std::sin(TAU * Float(slot) / Float(ADDITIVE::TABLE));
  synth->steps.resize(NOTES);
  for (Whole pitch = 0; pitch < NOTES; ++pitch)
    synth->steps[pitch] = increment(pitch, rate);
  for (Whole id = 0; id < ADDITIVE::PARAMETERS; ++id)
    ADDITIVE::apply(
      *synth, {AUDIO::PLUGIN::Event::CONTROLLER, 0, id, ADDITIVE::resting(id)});
  return synth;
}

auto meter(void *instance) -> Float {
  auto &synth = *static_cast<ADDITIVE::Synth *>(instance);
  return synth.level[synth.face.load(std::memory_order_acquire)];
}

void destroy(void *instance) {
  delete static_cast<ADDITIVE::Synth *>(instance);
}

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = ADDITIVE::render,
  .meter = meter,
  .destroy = destroy,
  .parameters = ADDITIVE::SURFACE::parameters,
  .name = ADDITIVE::SURFACE::name,
  .reading = ADDITIVE::SURFACE::reading,
  .held = ADDITIVE::SURFACE::held,
  .control = ADDITIVE::SURFACE::control,
  .ins = {{AUDIO::PLUGIN::Port::NOTES}, {AUDIO::PLUGIN::Port::CONTROL}},
  .outs = {{AUDIO::PLUGIN::Port::AUDIO}}};

[[maybe_unused]] const Flag offered =
  PLUG::offer({.name = "additive", .surface = &surface});

}  // namespace

auto SOUND::ADDITIVE::delta(Float span, Whole rate) -> Float {
  const Float frames = span * Float(rate);
  return frames <= 1.0f ? 1.0f : 1.0f / frames;
}

void SOUND::ADDITIVE::shape(Synth &synth) {
  synth.rise = delta(synth.attack, synth.rate);
  synth.fall = delta(synth.decay, synth.rate) * (1.0f - synth.sustain);
  synth.drop = delta(synth.release, synth.rate);
}
