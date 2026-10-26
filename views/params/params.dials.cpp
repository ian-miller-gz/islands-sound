// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../commands.hpp"
#include "../../control.hpp"
#include "../../graph.hpp"
#include "../../render.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "params.internal.hpp"
#include "params.overruled.hpp"

namespace {
using namespace SOUND;

auto instant() -> Whole {
  const TRANSPORT::Marker marker = TRANSPORT::marker();
  return marker.playing ? marker.position : TRANSPORT::mark();
}

}  // namespace

auto SOUND::VIEWS::RACK::described(const Row &row) -> AUDIO::PLUGIN::Control {
  const AUDIO::PLUGIN::Plug &surface = *GRAPH::offered(row.node)->surface;
  AUDIO::PLUGIN::Control published;
  if (
    surface.control == nullptr ||
    !surface.control(GRAPH::instance(row.node), row.parameter, published))
    return {};
  return published;
}

void SOUND::VIEWS::RACK::moved(const Row &row, Float value) {
  COMMANDS::dialled(row.node, row.parameter, value);
  COMMANDS::overrule(row.node, row.parameter);
}

auto SOUND::VIEWS::RACK::held(const Row &row) -> Float {
  const Address address = {.node = row.node, .parameter = row.parameter};
  const TIMELINE::Governed governed = RENDER::governed(address, ::instant());
  return CONTROL::sounding(
           address, {.governed = governed.stated,
                     .curve = governed.value,
                     .memory = GRAPH::remembered(row.node, row.parameter)})
    .value;
}

auto SOUND::VIEWS::RACK::shown(const Row &row) -> String {
  const AUDIO::PLUGIN::Plug &surface = *GRAPH::offered(row.node)->surface;
  void *instance = GRAPH::instance(row.node);
  const Float value = held(row);
  const Float adopted = GRAPH::remembered(row.node, row.parameter);
  const String reading = surface.reading == nullptr
                           ? String()
                           : surface.reading(instance, row.parameter);
  return value == adopted ? reading
                          : RIG::worded(described(row), value, reading);
}

void SOUND::VIEWS::RACK::turn(
  GUI::Handle page, const String &cell, const Row &row) {
  if (head(row)) return;
  const String dial = cell + "." + DIALLED;
  const AUDIO::PLUGIN::Control published = described(row);
  GUI::set(page, dial.c_str(), RIG::dressed(page, published));
  mark(page, cell, published);
  bound(page, cell, row);
  overruled(page, cell, row);
  typed(page, cell, row);
  if (GUI::GET::dialled(page, dial.c_str()))
    return RACK::dial(row, GUI::GET::value(page, dial.c_str()));
  GUI::set(page, dial.c_str(), GUI::Value{held(row)});
  if (String(GUI::GET::editing(page)) != dial || !GUI::GET::seated(page))
    GUI::set(page, dial.c_str(), GUI::Text{fitted(page, cell, row)});
}
