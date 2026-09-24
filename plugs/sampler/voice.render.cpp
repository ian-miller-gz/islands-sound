// SPDX-License-Identifier: AGPL-3.0-or-later
#include <xmmintrin.h>

#include "voice.internal.hpp"

namespace {
using namespace SOUND;

void denormals() { _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON); }

auto sampled(const Vector<Float> &line, Float place) -> Float {
  const Whole at = Whole(place);
  const Float here = line[at];
  const Float there = at + 1 < line.size() ? line[at + 1] : here;
  return here + (there - here) * (place - Float(at));
}

void sound(
  SAMPLER::Player &player, SAMPLER::Voice &voice,
  AUDIO::PLUGIN::Sample *const *lanes, Whole frame) {
  if (!voice.live) return;
  const SAMPLER::Source &source = player.sources[voice.source];
  const Float total = Float(SAMPLER::frames(source));
  if (voice.place >= total) {
    voice.place = player.loop == 0 ? total : voice.from + (voice.place - total);
    if (voice.place >= total) return (void)(voice.live = false);
  }
  const Float scale = voice.level * voice.velocity;
  for (Whole channel = 0; channel < player.channels; ++channel) {
    const Whole line = channel < source.lanes.size() ? channel : 0;
    lanes[channel][frame] += ::sampled(source.lanes[line], voice.place) * scale;
  }
  voice.place += voice.step;
  if (!voice.gated) {
    voice.level -= voice.fall;
    if (voice.level <= 0) voice.live = false;
    return;
  }
  voice.level += voice.rise;
  if (voice.level > 1) voice.level = 1;
}

void publish(SAMPLER::Player &player, Float peak) {
  const Whole idle = player.face.load(std::memory_order_relaxed) ^ 1u;
  player.level[idle] = peak;
  player.face.store(idle, std::memory_order_release);
}

}  // namespace

void SOUND::SAMPLER::render(
  void *instance, AUDIO::PLUGIN::Sample *const *lanes, Whole frames,
  const AUDIO::PLUGIN::Event *events, Whole count) {
  ::denormals();
  auto &player = *static_cast<Player *>(instance);
  Whole next = 0;
  Float peak = 0;
  for (Whole frame = 0; frame < frames; ++frame) {
    while (next < count && events[next].offset <= frame)
      apply(player, events[next++]);
    for (Whole channel = 0; channel < player.channels; ++channel)
      lanes[channel][frame] = 0;
    for (Voice &voice : player.voices) ::sound(player, voice, lanes, frame);
    for (Whole channel = 0; channel < player.channels; ++channel) {
      const Float sample = lanes[channel][frame] * player.gain;
      lanes[channel][frame] = sample;
      const Float magnitude = sample < 0 ? -sample : sample;
      if (magnitude > peak) peak = magnitude;
    }
  }
  while (next < count) apply(player, events[next++]);
  ::publish(player, peak);
}
