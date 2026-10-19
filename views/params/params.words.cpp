// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <format>

#include "../../control.hpp"
#include "../../graph.hpp"
#include "params.internal.hpp"

namespace {
using namespace SOUND;

constexpr Whole BASE = 10;
constexpr Whole SAID = 3;
constexpr Whole CHANNELED = 4;
constexpr Whole FIRST = 0;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, ::BASE);
}

auto row(const SHELL::Session &session) -> VIEWS::RACK::Row {
  return {
    .node = session.arguments[1], .parameter = ::counted(session.arguments[2])};
}

void arm(SHELL::Session &session) {
  if (session.arguments.size() < ::SAID)
    return session.print("arm <node> <parameter>");
  const Address address = VIEWS::RACK::addressed(::row(session));
  const STRING::Hot why = GRAPH::vetted(address.node, address.parameter);
  if (why[0] != '\0') return session.print(std::format("arm refused: {}", why));
  CONTROL::MAP::arm(address);
  session.print(std::format(
    "listening node {} parameter {}", address.node, address.parameter));
}

void disarm(SHELL::Session &session) {
  if (session.arguments.size() < ::SAID)
    return session.print("disarm <node> <parameter>");
  const VIEWS::RACK::Row held = ::row(session);
  const Address address = VIEWS::RACK::addressed(held);
  if (
    !VIEWS::RACK::listening(held) &&
    CONTROL::MAP::bound(address).number == NONE)
    return session.print("disarm refused: unbound");
  CONTROL::MAP::disarm(address);
  session.print(std::format(
    "disarmed node {} parameter {}", address.node, address.parameter));
}

void twist(SHELL::Session &session) {
  if (session.arguments.size() < ::SAID)
    return session.print("twist <number> <value> [channel]");
  const Whole number =
    std::min(::counted(session.arguments[1]), CONTROL::LOUDEST);
  const Whole value =
    std::min(::counted(session.arguments[2]), CONTROL::LOUDEST);
  const Whole channel = session.arguments.size() < ::CHANNELED
                          ? ::FIRST
                          : ::counted(session.arguments[3]);
  CONTROL::read(MIDI::Message{
    .kind = MIDI::Message::CONTROL,
    .channel = channel,
    .first = number,
    .second = value});
  session.print(std::format("twisted {} {}", number, value));
}

}  // namespace

auto SOUND::VIEWS::RACK::words() -> Vector<SHELL::Command> {
  return {
    {"arm", "", ::arm},
    {"disarm", "", ::disarm},
    {"twist", "", ::twist}};
}
