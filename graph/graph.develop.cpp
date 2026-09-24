// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

auto reached(const String &node, Whole out) -> GRAPH::Run * {
  if (!GRAPH::rooted(node)) return nullptr;
  Vector<GRAPH::Run> &runs =
    GRAPH::written(GRAPH::stands()[GRAPH::at(node)]).runs;
  return out < runs.size() ? &runs[out] : nullptr;
}

auto consumed(const String &node, Whole out) -> const GRAPH::Run * {
  if (!GRAPH::rooted(node)) return nullptr;
  const Vector<GRAPH::Run> &runs =
    GRAPH::walked(GRAPH::stands()[GRAPH::at(node)]).runs;
  return out < runs.size() ? &runs[out] : nullptr;
}

}  // namespace

auto SOUND::GRAPH::worn(const String &node, Whole kind) -> Whole {
  const Vector<Port> &outs = standing().nodes[at(node)].outs;
  for (Whole out = 0; out < outs.size(); ++out)
    if (outs[out].kind == kind) return out;
  return NONE;
}

auto SOUND::GRAPH::develop(
  const String &node, Whole out, Whole at, Whole pitch, Float velocity,
  Whole length) -> Flag {
  Run *run = ::reached(node, out);
  if (run == nullptr) return false;
  run->developed.push_back(
    {AUDIO::PLUGIN::Event::NOTE_ON, at, pitch, velocity});
  run->developed.push_back(
    {AUDIO::PLUGIN::Event::NOTE_OFF, at + length, pitch, 0});
  return true;
}

auto SOUND::GRAPH::develop(
  const String &node, Whole out, const AUDIO::PLUGIN::Event &edge) -> Flag {
  Run *run = ::reached(node, out);
  if (run == nullptr) return false;
  ordered(run->developed, edge);
  return true;
}

auto SOUND::GRAPH::develop(const String &node, const AUDIO::PLUGIN::Event &edge)
  -> Flag {
  if (!rooted(node)) return false;
  return develop(node, worn(node, KIND::NOTES), edge);
}

auto SOUND::GRAPH::develop(
  const String &node, Whole out, Whole at,
  const Vector<Vector<Float>> &lanes) -> Flag {
  Run *held = ::reached(node, out);
  if (held == nullptr) return false;
  Vector<Vector<Float>> &run = held->laid;
  run.resize(CHANNELS);
  for (Whole channel = 0; channel < CHANNELS && channel < lanes.size();
       ++channel) {
    const Vector<Float> &given = lanes[channel];
    if (run[channel].size() < at + given.size())
      run[channel].resize(at + given.size(), 0.0f);
    for (Whole frame = 0; frame < given.size(); ++frame)
      run[channel][at + frame] += given[frame];
  }
  return true;
}

auto SOUND::GRAPH::developed(const String &node, Whole out)
  -> const Vector<AUDIO::PLUGIN::Event> & {
  static const Vector<AUDIO::PLUGIN::Event> nothing;
  const Run *run = ::consumed(node, out);
  return run == nullptr ? nothing : run->developed;
}

auto SOUND::GRAPH::laid(const String &node, Whole out)
  -> const Vector<Vector<Float>> & {
  static const Vector<Vector<Float>> nothing;
  const Run *run = ::consumed(node, out);
  return run == nullptr ? nothing : run->laid;
}

void SOUND::GRAPH::clear(const String &node) {
  const Whole stood = at(node);
  if (stood == NONE) return;
  Tape &tape = written(stands()[stood]);
  for (Run &run : tape.runs) run = {};
  tape.dialled.clear();
}
