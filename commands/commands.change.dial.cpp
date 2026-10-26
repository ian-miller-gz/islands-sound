// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <sstream>

#include "../graph.hpp"
#include "../render.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto addressed(const Change &change) -> Address {
  Address address;
  std::istringstream said(change.at);
  said >> address.node >> address.parameter;
  return address;
}

auto instant() -> Whole {
  const TRANSPORT::Marker marker = TRANSPORT::marker();
  return marker.playing ? marker.position : TRANSPORT::mark();
}

}  // namespace

auto SOUND::COMMANDS::CHANGE::dialled(const Change &change) -> String {
  const Address address = ::addressed(change);
  if (GRAPH::vetted(address.node, address.parameter)[0] != '\0') return {};
  const TIMELINE::Governed governed = RENDER::governed(address, ::instant());
  const CONTROL::Sounded sounded = CONTROL::sounding(
    address, {.governed = governed.stated,
              .curve = governed.value,
              .memory = GRAPH::remembered(address.node, address.parameter)});
  return std::format("{}", sounded.value);
}

auto SOUND::COMMANDS::CHANGE::dialled(const Change &change, const String &value)
  -> Flag {
  const Address address = ::addressed(change);
  Float turned = 0.0f;
  std::istringstream said(value);
  if (!(said >> turned)) return false;
  if (GRAPH::hand(address.node, address.parameter, turned)[0] != '\0')
    return false;
  CONTROL::turn(address, turned);
  return true;
}

auto SOUND::COMMANDS::dialled(const String &node, Whole parameter, Float value)
  -> Flag {
  return kept(
    {.word = HISTORY::WORD::DIAL,
     .at = std::format("{} {}", node, parameter),
     .now = std::format("{}", value)});
}
