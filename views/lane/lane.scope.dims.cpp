// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>

#include "../../timeline.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;
namespace LANE = SOUND::VIEWS::LANE;

constexpr STRING::Hot WEST = "west";
constexpr STRING::Hot EAST = "east";
constexpr STRING::Hot RULER = "ruler";
constexpr Float ORIGIN = 0.0f;

struct Strip {
  Float x = 0.0f, y = 0.0f, deep = 0.0f;
};

auto named(STRING::Hot leaf) -> String {
  return String(LANE::face().name) + "." + leaf;
}

void laid(
  GUI::Handle page, STRING::Hot leaf, const Strip &strip, Float from,
  Float to) {
  const String id = ::named(leaf);
  GUI::set(page, id.c_str(), GUI::Visibility{to > from});
  if (to <= from) return;
  GUI::set(page, id.c_str(), GUI::Position{strip.x + from, strip.y});
  GUI::set(page, id.c_str(), GUI::Extent{to - from, strip.deep});
}

auto strip(GUI::Handle page, const String &board) -> Strip {
  const String ruler = String(LANE::faced()->name) + "." + ::RULER;
  const GUI::Position lane = GUI::GET::origin(page, LANE::face().name);
  const GUI::Position at = GUI::GET::origin(page, board.c_str());
  const GUI::Position top = GUI::GET::origin(page, ruler.c_str());
  const Float deep = GUI::GET::measured(page, board.c_str()).h;
  return {at.x - lane.x, top.y - lane.y, at.y + deep - top.y};
}

}  // namespace

void SOUND::VIEWS::LANE::dim() {
  const GUI::Handle page = document();
  for (STRING::Hot leaf : {::WEST, ::EAST})
    GUI::set(page, ::named(leaf).c_str(), GUI::Visibility{false});
}

void SOUND::VIEWS::LANE::dim(
  const Clip &clip, const Overview &read, Float rail) {
  const GUI::Handle page = document();
  const String board = field();
  const Float wide = GUI::GET::measured(page, board.c_str()).w;
  const Float pan = GUI::NGA::GET::pan(page, board.c_str()).x;
  const Float zoom = GUI::NGA::GET::zoom(page, board.c_str()).value;
  const Float floor = std::min(rail, wide);
  const auto seat = [&](Float at) {
    return std::clamp((at - pan) * zoom, floor, wide);
  };
  const Strip strip = ::strip(page, board);
  ::laid(page, ::WEST, strip, floor, seat(read.placed(clip.at)));
  ::laid(page, ::EAST, strip, seat(read.placed(clip.at + clip.frames)), wide);
}

auto SOUND::VIEWS::LANE::spanned(GUI::Handle page, const String &id, Float wide)
  -> Flag {
  const Clip *clip = GET::scoped();
  if (clip == nullptr || wide <= ::ORIGIN) return false;
  const Overview read = faced()->overview();
  if (read.placed == nullptr) return false;
  const Float opens = read.placed(clip->at);
  const Float closes = read.placed(clip->at + clip->frames);
  if (closes <= opens) return false;
  GUI::NGA::set(page, id.c_str(), GUI::NGA::Zoom{wide / (closes - opens)});
  GUI::NGA::set(page, id.c_str(), GUI::NGA::Pan{opens, ::ORIGIN});
  return true;
}
