// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../graph.hpp"
#include "../timeline.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto rooted(Whole track) -> String {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (track >= tracks.size()) return {};
  const String &root = tracks[track].root;
  return GRAPH::rooted(root) ? root : String();
}

void stood(const Laning &one) {
  if (!TIMELINE::lane(one.track, one.lane, one.row)) return;
  const String root = ::rooted(one.track);
  if (root.empty()) return;
  GRAPH::grow(root, one.port, one.lane);
  if (one.admits) GRAPH::admit(root, one.lane, true);
}

void shed(const Laning &one) {
  const String root = ::rooted(one.track);
  if (!TIMELINE::unlane(one.track, one.lane)) return;
  if (!root.empty()) GRAPH::shed(root, one.lane);
}

}  // namespace

void SOUND::COMMANDS::rowed(const Edit &edit, Flag standing) {
  if (standing) {
    for (const Row &one : edit.rows) TIMELINE::track(one.track, one.row);
    for (const Laning &one : edit.lanes) ::stood(one);
    return;
  }
  for (Whole one = edit.lanes.size(); one > 0; --one)
    ::shed(edit.lanes[one - 1]);
  for (Whole one = edit.rows.size(); one > 0; --one)
    TIMELINE::drop(edit.rows[one - 1].track);
}
