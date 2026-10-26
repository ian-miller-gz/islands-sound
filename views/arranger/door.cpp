// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../timeline.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::ARRANGER;
namespace LANE = SOUND::VIEWS::LANE;

constexpr Float WIDE = 176.0f;
constexpr Float FRAME = 12.0f;
constexpr Whole OFFER = 1;

const String DOOR = VIEWS::icon(NAME) + ".door";
const String PANEL = VIEWS::icon(NAME) + ".pages";
const String ROWS = ::PANEL + ".rows";

void session(const LANE::Mark &) {
  turned(SESSION);
  VIEWS::choose(NAME);
}

void edit(const LANE::Mark &mark) {
  VIEWS::ARRANGER::edit(mark.track, mark.lane);
}

using Take = void (*)(const LANE::Mark &);
constexpr Take TAKES[] = {::session, ::edit};

auto standing(GUI::Handle page) -> Flag {
  return GUI::GET::visibility(page, ::PANEL.c_str());
}

void plate(GUI::Handle page) {
  const LANE::Mark mark = offered();
  const Whole count = mark.lane == NONE ? ::OFFER : ::OFFER + 1;
  const Float step =
    GUI::GET::pitch(page, ::ROWS.c_str()) + GUI::GET::pad(page, ::ROWS.c_str());
  GUI::set(
    page, ::PANEL.c_str(), GUI::Extent{::WIDE, ::FRAME + Float(count) * step});
  GUI::set(page, ::ROWS.c_str(), GUI::Rows{count});
  GUI::set(
    page, std::format("{}.{}", ::ROWS, SESSION).c_str(),
    GUI::Text{PAGES[SESSION].label});
  if (count == ::OFFER) return;
  GUI::set(
    page, std::format("{}.{}", ::ROWS, ::OFFER).c_str(),
    GUI::Text{TIMELINE::held().tracks[mark.track].lanes[mark.lane].name});
}

void spread(GUI::Handle page, Flag on) {
  GUI::set(page, ::PANEL.c_str(), GUI::Visibility{on});
  if (!on) return;
  ::plate(page);
  const Whole row = PAGES[turned()].kind == NONE ? SESSION : ::OFFER;
  GUI::set(page, ::ROWS.c_str(), GUI::Cursor{row});
}

void took(GUI::Handle page) {
  const Whole row = GUI::GET::cursor(page, ::ROWS.c_str());
  const LANE::Mark mark = offered();
  if (row >= std::size(::TAKES) || (row == ::OFFER && mark.lane == NONE))
    return;
  ::TAKES[row](mark);
  ::spread(page, false);
}

}  // namespace

void SOUND::VIEWS::ARRANGER::door() {
  const GUI::Handle page = document();
  if (GUI::GET::clicked(page, ::DOOR.c_str()))
    return ::spread(page, !::standing(page));
  if (!::standing(page)) return;
  ::plate(page);
  if (GUI::GET::activated(page, ::ROWS.c_str())) return ::took(page);
  if (elsewhere(::DOOR.c_str(), ::PANEL.c_str())) ::spread(page, false);
}
