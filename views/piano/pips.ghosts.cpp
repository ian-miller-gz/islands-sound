// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "piano.notation.hpp"
#include "piano.pips.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr STRING::Hot BED = "ghost";
constexpr STRING::Hot PANEL = "panel";
constexpr STRING::Hot DRESS = "ghost";
constexpr STRING::Hot LINE = "ghosts {}";

VIEWS::Pool pool;
Vector<ROLL::Box> laid;

auto named(Whole at) -> String {
  return std::format("{}.{}.{}", ROLL::board(), ::BED, at);
}

auto staffed() -> Flag {
  return ROLL::GET::notation() == ROLL::PERCUSSION &&
         ROLL::GET::presentation() == ROLL::PRESENTATION::STAFF;
}

auto inside(const ROLL::Box &box, GUI::NGA::Pan pan, GUI::Extent seen) -> Flag {
  return box.at.x < pan.x + seen.w && box.at.x + box.size.w > pan.x &&
         box.at.y < pan.y + seen.h && box.at.y + box.size.h > pan.y;
}

auto others(Whole track, Whole edited) -> Vector<ROLL::Pip> {
  const auto &lanes = TIMELINE::held().tracks[track].lanes;
  Vector<ROLL::Pip> found;
  for (Whole lane = 0; lane < lanes.size(); ++lane) {
    if (lane == edited || lanes[lane].kind != KIND::NOTES) continue;
    for (const VIEWS::LANE::Clip &span : TIMELINE::spans(track, lane))
      ROLL::gathered(span, track, found);
  }
  return found;
}

void build(GUI::Handle page, Whole count) {
  const String board = ROLL::board();
  for (Whole at = count; at < ::pool.stood; ++at)
    BOARDS::drop(page, ::named(at));
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::named(at);
    BOARDS::place(page, board.c_str(), ::PANEL, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::DRESS});
    GUI::set(page, id.c_str(), GUI::Depth{ROLL::GHOST});
  }
  VIEWS::built(::pool, count);
}

}  // namespace

auto SOUND::VIEWS::ROLL::ghosts() -> Vector<Pip> {
  const LANE::Mark mark = LANE::edited(KIND::NOTES);
  if (mark.lane == NONE || ::staffed()) return {};
  const GUI::NGA::Pan pan = GUI::NGA::GET::pan(document(), board().c_str());
  const GUI::Extent seen = window();
  Vector<Pip> shown;
  for (const Pip &pip : ::others(mark.track, mark.lane))
    if (::inside(boxed(pip), pan, seen)) shown.push_back(pip);
  return shown;
}

void SOUND::VIEWS::ROLL::ghosts(GUI::Handle page) {
  const Flag again = VIEWS::reborn(::pool);
  Vector<Box> boxes;
  for (const Pip &pip : ghosts()) boxes.push_back(boxed(pip));
  if (!again && boxes == ::laid) return;
  const Whole seats = VIEWS::sized(::pool, boxes.size(), window().w);
  if (again || seats != ::pool.stood) ::build(page, seats);
  VIEWS::light(::pool, ::named, boxes.size());
  for (Whole at = 0; at < boxes.size(); ++at) {
    const String id = ::named(at);
    GUI::set(page, id.c_str(), boxes[at].at);
    GUI::set(page, id.c_str(), boxes[at].size);
  }
  ::laid = boxes;
}

void SOUND::VIEWS::ROLL::ghosts(SHELL::Session &session) {
  if (!::laid.empty()) session.print(std::format(::LINE, ::laid.size()));
}

void SOUND::VIEWS::ROLL::SHED::ghosts() {
  const GUI::Handle page = document();
  if (!VIEWS::reborn(::pool)) ::build(page, 0);
  ::laid.clear();
}
