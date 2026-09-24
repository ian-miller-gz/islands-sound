// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../control.hpp"
#include "../../graph.hpp"
#include "../../render.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "params.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot BARRED = "bar";

auto dressed(const AUDIO::PLUGIN::Control &published) -> GUI::Dial {
  return {
    .least = published.least,
    .most = published.most,
    .resting = published.resting,
    .steps = published.steps,
    .graphic = true};
}

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
  GRAPH::hand(row.node, row.parameter, value);
  CONTROL::turn({.node = row.node, .parameter = row.parameter}, value);
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

namespace {
auto worded(const AUDIO::PLUGIN::Control &published, Float value) -> String {
  if (published.steps == 0 || published.labels.size() != published.steps + 1)
    return std::format("{:g}{}", value, published.unit);
  return published.labels[VIEWS::RACK::stepped(published, value)];
}
}  // namespace

auto SOUND::VIEWS::RACK::shown(const Row &row) -> String {
  const AUDIO::PLUGIN::Plug &surface = *GRAPH::offered(row.node)->surface;
  void *instance = GRAPH::instance(row.node);
  const Float value = held(row);
  const Float adopted = GRAPH::remembered(row.node, row.parameter);
  if (value != adopted) return ::worded(described(row), value);
  return surface.reading == nullptr ? String()
                                    : surface.reading(instance, row.parameter);
}

void SOUND::VIEWS::RACK::turn(
  GUI::Handle page, const String &cell, const Row &row) {
  const String barred = cell + "." + ::BARRED;
  GUI::set(page, barred.c_str(), ::dressed(described(row)));
  if (GUI::GET::dialled(page, barred.c_str()))
    return moved(row, GUI::GET::value(page, barred.c_str()));
  GUI::set(page, barred.c_str(), GUI::Value{held(row)});
}
