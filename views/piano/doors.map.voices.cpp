// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../graph.hpp"
#include "piano.heads.hpp"
#include "piano.notation.hpp"

namespace {
using namespace SOUND;
using AUDIO::PLUGIN::Voice;
namespace ROLL = SOUND::VIEWS::ROLL;

auto voiced(const String &node) -> Vector<Voice> {
  Vector<Voice> own = GRAPH::GET::voices(node);
  return own.empty() ? ROLL::GET::drums() : own;
}

auto held(const Vector<Voice> &voices, const Voice &voice) -> Flag {
  return std::ranges::any_of(
    voices, [&](const Voice &one) { return one.note == voice.note; });
}

}  // namespace

auto SOUND::VIEWS::ROLL::GET::voices() -> Vector<AUDIO::PLUGIN::Voice> {
  Vector<Voice> merged;
  for (const String &node : mapped())
    for (const Voice &voice : ::voiced(node))
      if (!::held(merged, voice)) merged.push_back(voice);
  return merged;
}
