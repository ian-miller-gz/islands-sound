// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float SECOND = 1000.0f;

auto tuned(const SAMPLER::Player &player) -> Float {
  if (player.cents.empty()) return 1;
  const Float place = player.tune - player.lowest;
  const Whole at = Whole(place < 0 ? 0 : place);
  if (at + 1 >= player.cents.size()) return player.cents.back();
  return player.cents[at] +
         (player.cents[at + 1] - player.cents[at]) * (place - Float(at));
}

auto seat(SAMPLER::Player &player) -> SAMPLER::Voice & {
  Whole chosen = 0;
  for (Whole index = 0; index < SAMPLER::VOICES; ++index) {
    if (!player.voices[index].live) return player.voices[index];
    if (player.voices[index].level < player.voices[chosen].level)
      chosen = index;
  }
  return player.voices[chosen];
}

void strike(SAMPLER::Player &player, Whole pitch, Float velocity) {
  const Whole root = Whole(player.root);
  if (player.chosen >= player.sources.size()) return;
  if (pitch >= player.steps.size() || root >= player.steps.size()) return;
  const SAMPLER::Source &source = player.sources[player.chosen];
  const Float total = Float(SAMPLER::frames(source));
  const Float from = player.start * player.ratio * Float(player.rate) / SECOND;
  if (total == 0 || from >= total) return;
  SAMPLER::Voice &voice = ::seat(player);
  voice.source = player.chosen;
  voice.base = player.ratio * player.steps[pitch] / player.steps[root];
  voice.step = voice.base * player.tuning;
  voice.place = from;
  voice.from = from;
  voice.level = 0;
  voice.rise = SAMPLER::delta(player.attack, player.rate);
  voice.fall = SAMPLER::delta(player.release, player.rate);
  voice.velocity = velocity > 1 ? 1 : velocity;
  voice.pitch = pitch;
  voice.gated = true;
  voice.live = true;
}

void lift(SAMPLER::Player &player, Whole pitch) {
  for (SAMPLER::Voice &voice : player.voices) {
    if (!voice.live || !voice.gated || voice.pitch != pitch) continue;
    voice.gated = false;
    voice.fall = SAMPLER::delta(player.release, player.rate);
  }
}

auto picked(const SAMPLER::Player &player, Float value) -> Whole {
  if (player.sources.empty()) return 0;
  const Float most = Float(player.sources.size() - 1);
  return Whole(value < 0 ? 0 : value > most ? most : value);
}

}  // namespace

void SOUND::SAMPLER::steer(Player &player, Whole id, Float value) {
  if (id == STOCK) {
    player.chosen = ::picked(player, value);
    return steer(player, START, player.start);
  }
  if (id == START) {
    const Float most = span(player);
    player.start = value < 0 ? 0 : value > most ? most : value;
    return;
  }
  const Float held = clamped(id, value);
  if (id == GAIN) player.gain = held;
  if (id == ROOT) player.root = held;
  if (id == LOOP) player.loop = held;
  if (id == ATTACK) player.attack = held;
  if (id == RELEASE) player.release = held;
  if (id == TUNE) {
    player.tune = held;
    player.tuning = ::tuned(player);
    for (Voice &voice : player.voices) voice.step = voice.base * player.tuning;
  }
}

void SOUND::SAMPLER::apply(Player &player, const AUDIO::PLUGIN::Event &event) {
  if (event.kind == AUDIO::PLUGIN::Event::CONTROLLER)
    return steer(player, event.index, event.value);
  if (event.kind == AUDIO::PLUGIN::Event::PROGRAM)
    return steer(player, STOCK, Float(event.index));
  if (event.kind == AUDIO::PLUGIN::Event::NOTE_OFF)
    return ::lift(player, event.index);
  if (event.kind != AUDIO::PLUGIN::Event::NOTE_ON || event.value <= 0) return;
  ::strike(player, event.index, event.value);
}
