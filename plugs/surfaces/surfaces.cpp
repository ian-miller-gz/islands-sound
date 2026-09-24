// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <island/audio.hpp>
#include <island/midi.hpp>
#include "surfaces.hpp"
#include "surfaces.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::SURFACES;

auto hearing(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  return new Input{.rate = rate, .channels = channels, .lanes = channels};
}
auto sounding(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  return new Output{.rate = rate, .channels = channels};
}
auto speaking(Whole rate, Whole channels) -> void * {
  if (rate == 0 || channels == 0) return nullptr;
  return new Midiout{};
}

void unheard(void *instance) {
  auto *input = static_cast<Input *>(instance);
  if (input->stream != AUDIO::INPUT::NONE) AUDIO::INPUT::remove(input->stream);
  delete input;
}
void unsounded(void *instance) {
  auto *output = static_cast<Output *>(instance);
  if (output->stream != AUDIO::OUTPUT::NONE)
    AUDIO::OUTPUT::remove(output->stream);
  delete output;
}
void unspoken(void *instance) {
  auto *out = static_cast<Midiout *>(instance);
  if (out->output != MIDI::OUTPUT::NONE) MIDI::OUTPUT::remove(out->output);
  delete out;
}

auto parameters(void *) -> Whole { return 0; }
auto name(void *, Whole) -> String { return {}; }
auto reading(void *, Whole) -> String { return {}; }
auto held(void *, Whole) -> Float { return 0.0f; }
auto control(void *, Whole, AUDIO::PLUGIN::Control &) -> Flag { return false; }

const AUDIO::PLUGIN::Plug input = {
  .create = hearing,
  .render = SURFACES::heard,
  .meter = nullptr,
  .destroy = unheard,
  .parameters = parameters,
  .name = name,
  .reading = reading,
  .held = held,
  .control = control,
  .ins = {},
  .outs = {{AUDIO::PLUGIN::Port::AUDIO}}};
const AUDIO::PLUGIN::Plug output = {
  .create = sounding,
  .render = SURFACES::sounded,
  .meter = nullptr,
  .destroy = unsounded,
  .parameters = parameters,
  .name = name,
  .reading = reading,
  .held = held,
  .control = control,
  .ins = {{AUDIO::PLUGIN::Port::AUDIO}},
  .outs = {}};
const AUDIO::PLUGIN::Plug midiout = {
  .create = speaking,
  .render = SURFACES::spoken,
  .meter = nullptr,
  .destroy = unspoken,
  .parameters = parameters,
  .name = name,
  .reading = reading,
  .held = held,
  .control = control,
  .ins = {{AUDIO::PLUGIN::Port::NOTES}},
  .outs = {}};

[[maybe_unused]] const Flag offers =
  PLUG::offer(
    {.name = SURFACES::INPUT, .surface = &input, .bind = SURFACES::hear}) &&
  PLUG::offer(
    {.name = SURFACES::OUTPUT, .surface = &output, .bind = SURFACES::sound}) &&
  PLUG::offer(
    {.name = SURFACES::MIDIOUT, .surface = &midiout, .bind = SURFACES::speak});

}  // namespace
