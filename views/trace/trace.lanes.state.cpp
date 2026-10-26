// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "../lane/lane.face.hpp"
#include "trace.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot SHOWN = "shown";
constexpr STRING::Hot HIDDEN = "hidden";

auto admits(Whole track, Whole lane) -> Flag {
  const String root = TIMELINE::held().tracks[track].root;
  return GRAPH::rooted(root) && GRAPH::admitted(root, lane) != NONE;
}

}  // namespace

void SOUND::VIEWS::TRACE::lanes(SHELL::Session &session) {
  const Whole track = attended();
  const Whole count =
    track == NONE ? 0 : TIMELINE::held().tracks[track].lanes.size();
  session.print(std::format(
    "lanes {} rows {} kind {}",
    String(GET::list()) == LANES ? ::SHOWN : ::HIDDEN, count,
    KIND::spoken(adding())));
  for (Whole lane = 0; lane < count; ++lane) {
    const ARRANGEMENT::TRACK::Lane &held =
      TIMELINE::held().tracks[track].lanes[lane];
    session.print(std::format(
      "lane {} {} {} admits {}", lane, held.name, KIND::spoken(held.kind),
      ::admits(track, lane) ? "on" : "off"));
  }
  LANE::marked(session);
}
