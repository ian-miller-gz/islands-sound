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
  for (const PLUGIN::Offer &row : PLUGIN::catalog())
    if (row.bind == nullptr) names.push_back(row.name);
  return names;
}

auto SOUND::GRAPH::surfaces() -> Vector<String> {
  Vector<String> names;
  for (const PLUGIN::Offer &row : PLUGIN::catalog())
    if (row.bind != nullptr) names.push_back(row.name);
  return names;
}

auto SOUND::GRAPH::bound(const String &plugin) -> Flag {
  return PLUGIN::bound(plugin);
}

auto SOUND::GRAPH::takes(const String &name) -> Vector<Whole> {
  const PLUGIN::Offer *row = PLUGIN::found(name);
  return row == nullptr ? Vector<Whole>{} : ::kinded(row->surface->ins);
}

auto SOUND::GRAPH::gives(const String &name) -> Vector<Whole> {
  const PLUGIN::Offer *row = PLUGIN::found(name);
  return row == nullptr ? Vector<Whole>{} : ::kinded(row->surface->outs);
}

auto SOUND::GRAPH::from(const String &name) -> STRING::Hot {
  const PLUGIN::Offer *row = PLUGIN::found(name);
  return row == nullptr ? "" : row->from;
}

auto SOUND::GRAPH::typed(const String &name) -> STRING::Hot {
  const PLUGIN::Offer *row = PLUGIN::found(name);
  return row == nullptr ? "" : row->type;
}

auto SOUND::GRAPH::voiced(const String &name) -> STRING::Hot {
  const PLUGIN::Offer *row = PLUGIN::found(name);
  return row == nullptr ? "" : row->voicing;
}
