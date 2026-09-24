// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../control.hpp"
#include "../graph.hpp"
#include "../inventory.hpp"
#include "../render.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto instant() -> Whole {
  const TRANSPORT::Marker marker = TRANSPORT::marker();
  return marker.playing ? marker.position : TRANSPORT::mark();
}

void standing(SHELL::Session &session) {
  session.print(std::format("values {}", CONTROL::held().size()));
  for (const Value &value : CONTROL::held())
    session.print(std::format(
      "value node {} parameter {} {:.3f}", value.address.node,
      value.address.parameter, value.value));
}

}  // namespace

void SOUND::COMMANDS::sounding(SHELL::Session &session) {
  if (session.arguments.size() < 3) return ::standing(session);
  const Address address = {
    .node = session.arguments[1], .parameter = ::counted(session.arguments[2])};
  const Whole at = ::instant();
  const TIMELINE::Governed governed = RENDER::governed(address, at);
  const CONTROL::Sounded sounded = CONTROL::sounding(
    address, {.governed = governed.stated,
              .curve = governed.value,
              .memory = GRAPH::remembered(address.node, address.parameter)});
  session.print(std::format(
    "sounding node {} parameter {} at {} value {:.3f} from {}", address.node,
    address.parameter, at, sounded.value, sounded.from));
}
