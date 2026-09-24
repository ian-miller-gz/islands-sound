// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../graph.hpp"
#include "../master.hpp"
#include "../commands.hpp"

namespace {

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

}  // namespace

void SOUND::COMMANDS::note(SHELL::Session &session) {
  if (session.arguments.size() < 7)
    return session.print("note <node> <out> <at> <pitch> <velocity> <length>");
  const String &node = session.arguments[1];
  const Whole out = ::counted(session.arguments[2]);
  const Whole at = ::counted(session.arguments[3]);
  const Whole pitch = ::counted(session.arguments[4]);
  const Float velocity = std::strtof(session.arguments[5].c_str(), nullptr);
  if (!GRAPH::develop(
        node, out, at, pitch, velocity, ::counted(session.arguments[6])))
    return session.print("no such root or out");
  GRAPH::sort();
  session.print(std::format(
    "noted {} out {} pitch {} at {} velocity {:.3f} run {}", node, out, pitch,
    at, velocity, GRAPH::developed(node, out).size()));
}

void SOUND::COMMANDS::turn(SHELL::Session &session) {
  if (session.arguments.size() < 6)
    return session.print("turn <node> <out> <at> <number> <value>");
  const String &node = session.arguments[1];
  const Whole out = ::counted(session.arguments[2]);
  const Whole at = ::counted(session.arguments[3]);
  const Whole number = ::counted(session.arguments[4]);
  const Float value = std::strtof(session.arguments[5].c_str(), nullptr);
  if (!GRAPH::develop(
        node, out, {AUDIO::PLUGIN::Event::CONTROLLER, at, number, value}))
    return session.print("no such root or out");
  session.print(std::format(
    "turned {} out {} number {} at {} value {:.3f} run {}", node, out, number,
    at, value, GRAPH::developed(node, out).size()));
}

void SOUND::COMMANDS::bounce(SHELL::Session &session) {
  if (session.arguments.size() < 2)
    return session.print("bounce <frames> [from]");
  MASTER::Tally tally;
  const Whole from =
    session.arguments.size() > 2 ? ::counted(session.arguments[2]) : 0;
  if (!MASTER::bounce(from, ::counted(session.arguments[1]), tally))
    return session.print("bounce refused: the graph has a cycle");
  session.print(std::format(
    "bounced {} frames {:.3f} s {} Hz {} channels notes {} digest {:08x}{}",
    tally.frames, Float(tally.frames) / Float(tally.rate), tally.rate,
    tally.channels, tally.notes, tally.digest,
    from == 0 ? String() : std::format(" from {}", from)));
}
