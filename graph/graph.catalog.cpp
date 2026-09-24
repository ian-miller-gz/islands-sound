// SPDX-License-Identifier: AGPL-3.0-or-later
#include "graph.hpp"

namespace {
using namespace SOUND;

auto kinded(const Vector<AUDIO::PLUGIN::Port> &declared) -> Vector<Whole> {
  Vector<Whole> kinds;
  for (const AUDIO::PLUGIN::Port &port : declared) kinds.push_back(port.kind);
  return kinds;
}

}  // namespace

auto SOUND::GRAPH::offers() -> Vector<String> {
  Vector<String> names;
  for (const PLUG::Offer &row : PLUG::catalog())
    if (row.bind == nullptr) names.push_back(row.name);
  return names;
}

auto SOUND::GRAPH::surfaces() -> Vector<String> {
  Vector<String> names;
  for (const PLUG::Offer &row : PLUG::catalog())
    if (row.bind != nullptr) names.push_back(row.name);
  return names;
}

auto SOUND::GRAPH::bound(const String &plug) -> Flag {
  return PLUG::bound(plug);
}

auto SOUND::GRAPH::takes(const String &name) -> Vector<Whole> {
  const PLUG::Offer *row = PLUG::found(name);
  return row == nullptr ? Vector<Whole>{} : ::kinded(row->surface->ins);
}

auto SOUND::GRAPH::gives(const String &name) -> Vector<Whole> {
  const PLUG::Offer *row = PLUG::found(name);
  return row == nullptr ? Vector<Whole>{} : ::kinded(row->surface->outs);
}

auto SOUND::GRAPH::from(const String &name) -> STRING::Hot {
  const PLUG::Offer *row = PLUG::found(name);
  return row == nullptr ? "" : row->from;
}
