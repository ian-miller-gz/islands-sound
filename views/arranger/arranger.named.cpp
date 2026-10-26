// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../timeline.hpp"
#include "../lane/lane.face.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LEAF = "lane";
constexpr STRING::Hot SEPARATOR = " · ";

const String LABEL = String(VIEWS::ARRANGER::NAME) + "." + ::LEAF;

auto named(const VIEWS::LANE::Mark &mark) -> String {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (mark.track >= tracks.size()) return {};
  const ARRANGEMENT::Track &track = tracks[mark.track];
  if (mark.lane >= track.lanes.size()) return {};
  return track.name + ::SEPARATOR + track.lanes[mark.lane].name;
}

}  // namespace

void SOUND::VIEWS::ARRANGER::label(GUI::Handle page) {
  const Whole kind = PAGES[turned()].kind;
  const String text = kind == NONE ? String() : ::named(LANE::edited(kind));
  GUI::set(page, ::LABEL.c_str(), GUI::Visibility{!text.empty()});
  GUI::set(page, ::LABEL.c_str(), GUI::Text{text});
}
