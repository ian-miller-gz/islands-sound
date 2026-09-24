// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <island/audio.hpp>
#include <island/midi.hpp>

#include "surfaces.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::SURFACES;
}  // namespace

void SOUND::SURFACES::heard(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *, Whole) {
  auto &input = *static_cast<Input *>(instance);
  if (input.stream == AUDIO::INPUT::NONE || input.scratch.empty()) return;
  const Whole room =
    std::min<Whole>(frames, input.scratch.size() / input.lanes);
  input.scratch.resize(room * input.lanes);
  const Whole got = AUDIO::INPUT::read(input.stream, input.scratch);
  for (Whole frame = 0; frame < got; ++frame)
    for (Whole channel = 0; channel < input.channels; ++channel) {
      const Whole lane = std::min(input.lane + channel, input.lanes - 1);
      lanes[channel][frame] =
        Float(input.scratch[frame * input.lanes + lane]) / FULL;
    }
}

void SOUND::SURFACES::sounded(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *, Whole) {
  auto &output = *static_cast<Output *>(instance);
  if (output.stream == AUDIO::OUTPUT::NONE || output.lanes == 0) return;
  const Whole room =
    std::min<Whole>(frames, output.scratch.capacity() / output.lanes);
  output.scratch.assign(room * output.lanes, 0);
  for (Whole frame = 0; frame < room; ++frame)
    for (Whole channel = 0; channel < output.channels; ++channel) {
      const Whole lane = std::min(output.lane + channel, output.lanes - 1);
      const Float held = std::clamp(lanes[channel][frame], -1.0f, 1.0f);
      output.scratch[frame * output.lanes + lane] =
        static_cast<AUDIO::Sample>(held * (FULL - 1.0f));
    }
  AUDIO::OUTPUT::feed(output.stream, output.scratch);
}

void SOUND::SURFACES::spoken(
  void *instance, AUDIO::PLUGIN::Sample *const *, Whole,
  const AUDIO::PLUGIN::Event *events, Whole count) {
  auto &out = *static_cast<Midiout *>(instance);
  if (out.output == MIDI::OUTPUT::NONE) return;
  for (Whole at = 0; at < count; ++at) {
    const AUDIO::PLUGIN::Event &event = events[at];
    const Flag on = event.kind == AUDIO::PLUGIN::Event::NOTE_ON;
    if (!on && event.kind != AUDIO::PLUGIN::Event::NOTE_OFF) continue;
    const Whole velocity =
      std::clamp<Whole>(Whole(event.value * PEAK + 0.5f), 0, Whole(PEAK));
    MIDI::OUTPUT::send(
      out.output, {on ? MIDI::Message::NOTE_ON : MIDI::Message::NOTE_OFF, 0,
                   event.index, on ? velocity : 0});
  }
}
