// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Whole NOTES = 128;
constexpr Float SEMITONES = 12;
constexpr Float OCTAVE = 1200;
constexpr Float STRIDE = 1;

auto create(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  auto *player = new SAMPLER::Player{.rate = rate, .channels = channels};
  SAMPLER::stock(*player);
  player->steps.resize(NOTES);
  for (Whole note = 0; note < NOTES; ++note)
    player->steps[note] = std::pow(2.0f, Float(note) / SEMITONES);
  AUDIO::PLUGIN::Control tuning;
  SAMPLER::control(SAMPLER::TUNE, tuning);
  player->lowest = tuning.least;
  for (Float cent = tuning.least; cent <= tuning.most; cent += STRIDE)
    player->cents.push_back(std::pow(2.0f, cent / OCTAVE));
  for (Whole id = 0; id < SAMPLER::PARAMETERS; ++id)
    SAMPLER::apply(
      *player, {AUDIO::PLUGIN::Event::CONTROLLER, 0, id, SAMPLER::resting(id)});
  return player;
}

auto meter(void *instance) -> Float {
  auto &player = *static_cast<SAMPLER::Player *>(instance);
  return player.level[player.face.load(std::memory_order_acquire)];
}

void destroy(void *instance) {
  delete static_cast<SAMPLER::Player *>(instance);
}

const AUDIO::PLUGIN::Plug surface = {
  .create = create,
  .render = SAMPLER::render,
  .meter = meter,
  .destroy = destroy,
  .parameters = SAMPLER::SURFACE::parameters,
  .name = SAMPLER::SURFACE::name,
  .reading = SAMPLER::SURFACE::reading,
  .held = SAMPLER::SURFACE::held,
  .control = SAMPLER::SURFACE::control,
  .ins =
    {{AUDIO::PLUGIN::Port::NOTES, "notes"},
     {AUDIO::PLUGIN::Port::CONTROL, "turns"},
     {AUDIO::PLUGIN::Port::DATA, "material", "sample"},
     {AUDIO::PLUGIN::Port::PROGRAM, "programs"}},
  .outs = {
    {AUDIO::PLUGIN::Port::AUDIO, "signal"},
    {AUDIO::PLUGIN::Port::DATA, "material", "sample"}}};

[[maybe_unused]] const Flag offered =
  PLUG::offer({.name = "sampler", .surface = &surface});

}  // namespace

auto SOUND::SAMPLER::delta(Float seconds, Whole rate) -> Float {
  const Float frames = seconds * Float(rate);
  return frames <= 1.0f ? 1.0f : 1.0f / frames;
}
