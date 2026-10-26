// SPDX-License-Identifier: AGPL-3.0-or-later
#include "render.internal.hpp"

namespace SOUND::RENDER::STRUCK {
namespace {
using namespace SOUND;

struct Rail {
  String root;
  Whole out = 0;
  Flag held[GRAPH::PITCHES] = {};
};

Vector<Rail> rails;
Whole ahead = NONE;

auto rail(const String &root, Whole out) -> Rail & {
  for (Rail &one : rails)
    if (one.out == out && one.root == root) return one;
  rails.push_back({root, out});
  return rails.back();
}

void opened(
  const String &root, Whole out, Whole before,
  Flag (&pitches)[GRAPH::PITCHES]) {
  for (const AUDIO::PLUGIN::Event &edge : GRAPH::developed(root, out)) {
    if (edge.offset >= before) break;
    if (edge.index >= GRAPH::PITCHES) continue;
    if (edge.kind == AUDIO::PLUGIN::Event::NOTE_ON) pitches[edge.index] = true;
    if (edge.kind == AUDIO::PLUGIN::Event::NOTE_OFF)
      pitches[edge.index] = false;
  }
}

void ended(
  Rail &rail, Whole pitch, Whole at, Vector<AUDIO::PLUGIN::Event> &into) {
  rail.held[pitch] = false;
  into.push_back({AUDIO::PLUGIN::Event::NOTE_OFF, at, pitch, 0.0f});
}

}  // namespace
}  // namespace SOUND::RENDER::STRUCK

void SOUND::RENDER::STRUCK::struck(
  const String &root, Whole out, const AUDIO::PLUGIN::Event &edge) {
  if (edge.index >= GRAPH::PITCHES) return;
  if (edge.kind == AUDIO::PLUGIN::Event::NOTE_ON)
    rail(root, out).held[edge.index] = true;
  if (edge.kind == AUDIO::PLUGIN::Event::NOTE_OFF)
    rail(root, out).held[edge.index] = false;
}

void SOUND::RENDER::STRUCK::lifted(const String &root, Whole out, Whole pitch) {
  if (pitch < GRAPH::PITCHES) rail(root, out).held[pitch] = false;
}

void SOUND::RENDER::STRUCK::cut(
  const String &root, Whole out, Whole at, Vector<AUDIO::PLUGIN::Event> &into) {
  Rail &held = rail(root, out);
  for (Whole pitch = 0; pitch < GRAPH::PITCHES; ++pitch)
    if (held.held[pitch]) ended(held, pitch, at, into);
}

void SOUND::RENDER::STRUCK::swapped(
  const String &root, Whole out, Whole start,
  Vector<AUDIO::PLUGIN::Event> &into) {
  Rail &held = rail(root, out);
  Flag open[GRAPH::PITCHES] = {};
  opened(root, out, start, open);
  for (Whole pitch = 0; pitch < GRAPH::PITCHES; ++pitch)
    if (held.held[pitch] && !open[pitch]) ended(held, pitch, 0, into);
}

auto SOUND::RENDER::STRUCK::jumped(const Walk &walk, Whole frames) -> Flag {
  const Flag away = walk.start != NONE && ahead != NONE && walk.start != ahead;
  ahead = after(walk, frames);
  return away;
}

void SOUND::RENDER::forget() {
  STRUCK::rails.clear();
  STRUCK::ahead = NONE;
}
