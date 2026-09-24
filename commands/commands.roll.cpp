// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../graph.hpp"
#include "../inventory.hpp"
#include "../kind.hpp"
#include "../render.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

void cleared() {
  for (const Node &node : GRAPH::held().nodes) GRAPH::clear(node.name);
}

void fed(const Vector<TIMELINE::Sounding> &run, Whole from) {
  for (const TIMELINE::Sounding &note : run)
    GRAPH::develop(
      note.root, note.lane, note.at - from, note.pitch, note.velocity,
      note.length);
}

void fed(const Vector<TIMELINE::Laid> &run) {
  for (const TIMELINE::Laid &stretch : run)
    GRAPH::develop(stretch.root, stretch.lane, stretch.at, stretch.lanes);
}

void fed(const Vector<TIMELINE::Sent> &run, Whole from) {
  for (const TIMELINE::Sent &edge : run)
    GRAPH::develop(edge.root, edge.lane, RENDER::edged(edge, edge.at - from));
}

void fed(const Vector<TIMELINE::Dialled> &run, Whole from) {
  for (const TIMELINE::Dialled &turn : run)
    GRAPH::dial(
      turn.address.node, turn.at - from, turn.address.parameter, turn.value);
}

void sent(SHELL::Session &session, const Vector<TIMELINE::Sent> &run) {
  if (run.empty()) return;
  Whole controls = 0;
  for (const TIMELINE::Sent &edge : run)
    controls += edge.kind == KIND::CONTROL ? 1 : 0;
  session.print(std::format(
    "sent {} controls {} programs", controls, run.size() - controls));
}

void sounded(SHELL::Session &session, const Vector<TIMELINE::Sounding> &run) {
  for (const TIMELINE::Sounding &note : run)
    session.print(std::format(
      "sounding track {} root {} at {} pitch {} velocity {:.3f} for {}",
      note.track, note.root, note.at, note.pitch, note.velocity, note.length));
}

void stretched(
  SHELL::Session &session, Whole from, const Vector<TIMELINE::Laid> &run) {
  for (const TIMELINE::Laid &stretch : run)
    session.print(std::format(
      "laid track {} root {} at {} for {} frames", stretch.track, stretch.root,
      stretch.at + from, stretch.lanes.empty() ? 0 : stretch.lanes[0].size()));
}

}  // namespace

void SOUND::COMMANDS::roll(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("roll <frames>");
  const Whole frames = ::counted(session.arguments[1]);
  const Whole from = TRANSPORT::marker().position;
  ::cleared();
  const Vector<TIMELINE::Sounding> run = TIMELINE::roll(
    INVENTORY::held(), from, frames, TRANSPORT::held().tempos, {});
  const Vector<TIMELINE::Laid> laid =
    TIMELINE::lay(INVENTORY::held(), from, frames, {});
  const Vector<TIMELINE::Dialled> turns =
    TIMELINE::dial(INVENTORY::held(), from, frames, RENDER::BLOCK, {});
  const Vector<TIMELINE::Sent> edges =
    RENDER::sent(INVENTORY::held(), from, frames, {});
  ::fed(run, from);
  ::fed(laid);
  ::fed(edges, from);
  ::fed(turns, from);
  GRAPH::sort();
  session.print(std::format(
    "rolled {} notes {} stretches {} turns from {} for {} at {:.3f} BPM",
    run.size(), laid.size(), turns.size(), from, frames, TRANSPORT::paced()));
  ::sent(session, edges);
  ::sounded(session, run);
  ::stretched(session, from, laid);
}
