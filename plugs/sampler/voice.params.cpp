// SPDX-License-Identifier: AGPL-3.0-or-later
#include "voice.internal.hpp"

namespace {
using namespace SOUND;

auto quantity(const SAMPLER::Player &player, Whole index) -> Float {
  if (index == SAMPLER::GAIN) return player.gain;
  if (index == SAMPLER::STOCK) return Float(player.chosen);
  if (index == SAMPLER::ROOT) return player.root;
  if (index == SAMPLER::TUNE) return player.tune;
  if (index == SAMPLER::START) return player.start;
  if (index == SAMPLER::LOOP) return player.loop;
  if (index == SAMPLER::ATTACK) return player.attack;
  return player.release;
}

auto sane(void *instance, Whole index) -> Flag {
  return instance != nullptr && index < SAMPLER::PARAMETERS;
}

void chooser(const SAMPLER::Player &player, AUDIO::PLUGIN::Control &out) {
  if (player.sources.size() < 2) return;
  out.steps = player.sources.size() - 1;
  out.most = Float(out.steps);
  for (const SAMPLER::Source &source : player.sources)
    out.labels.push_back(source.name);
}

}  // namespace

auto SOUND::SAMPLER::SURFACE::parameters(void *instance) -> Whole {
  return instance == nullptr ? 0 : PARAMETERS;
}

auto SOUND::SAMPLER::SURFACE::name(void *instance, Whole index) -> String {
  return ::sane(instance, index) ? label(index) : String();
}

auto SOUND::SAMPLER::SURFACE::reading(void *instance, Whole index) -> String {
  if (!::sane(instance, index)) return {};
  if (index != STOCK) return notation(index, held(instance, index));
  const auto &player = *static_cast<const Player *>(instance);
  return player.chosen < player.sources.size()
           ? player.sources[player.chosen].name
           : String("none");
}

auto SOUND::SAMPLER::SURFACE::held(void *instance, Whole index) -> Float {
  if (!::sane(instance, index)) return 0;
  return ::quantity(*static_cast<const Player *>(instance), index);
}

auto SOUND::SAMPLER::SURFACE::control(
  void *instance, Whole index, AUDIO::PLUGIN::Control &out) -> Flag {
  if (!::sane(instance, index) || !SAMPLER::control(index, out)) return false;
  const auto &player = *static_cast<const Player *>(instance);
  if (index == STOCK) ::chooser(player, out);
  if (index == START) out.most = span(player);
  return true;
}
