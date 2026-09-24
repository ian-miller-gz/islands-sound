// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "../../control.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot COVER = "cover";
constexpr Float OVER = 2.0f;

Vector<String> seated;
Whole born = 0;

struct Landing {
  Whole kind = KIND::AUDIO, lane = NONE, band = NONE;
};

auto id(const Landing &fall) -> String {
  return VIEWS::ARRANGEMENT::board() + "." + ::COVER + "." +
         KIND::spoken(fall.kind) + "." + std::to_string(fall.lane);
}

auto laned(Whole track, Whole kind) -> Whole {
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes =
    TIMELINE::held().tracks[track].lanes;
  for (Whole lane = 0; lane < lanes.size(); ++lane)
    if (lanes[lane].kind == kind) return lane;
  return NONE;
}

auto crossed(Whole track, Whole lane) -> Whole {
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes =
    TIMELINE::held().tracks[track].lanes;
  if (lane < lanes.size() && lanes[lane].kind == KIND::AUDIO) return lane;
  return ::laned(track, KIND::AUDIO);
}

void fallen(Vector<Landing> &falls, Whole track, Whole kind, Whole lane) {
  if (lane == NONE) return;
  for (const Landing &held : falls)
    if (held.kind == kind && held.lane == lane) return;
  falls.push_back({kind, lane, VIEWS::ARRANGEMENT::banded(track, lane)});
}

auto landings(const CONTROL::Reel &reel) -> Vector<Landing> {
  Vector<Landing> falls;
  const Whole track = CONTROL::driven();
  if (!reel.taking || track >= TIMELINE::held().tracks.size()) return falls;
  if (reel.heard > 0)
    ::fallen(falls, track, KIND::AUDIO, ::laned(track, KIND::AUDIO));
  for (const Whole lane : reel.crossed)
    ::fallen(falls, track, KIND::AUDIO, ::crossed(track, lane));
  if (reel.notes > 0)
    ::fallen(falls, track, KIND::NOTES, ::laned(track, KIND::NOTES));
  if (reel.turned > 0)
    ::fallen(falls, track, KIND::LOGIC, ::laned(track, KIND::LOGIC));
  return falls;
}

void seat(GUI::Handle page, const Landing &fall) {
  const String plate = ::id(fall);
  for (const String &held : ::seated)
    if (held == plate) return;
  BOARDS::place(
    page, VIEWS::ARRANGEMENT::board().c_str(), "panel", plate, {}, {});
  GUI::set(
    page, plate.c_str(),
    GUI::Style{VIEWS::ARRANGEMENT::plated(fall.kind).c_str()});
  GUI::set(page, plate.c_str(), GUI::Depth{::OVER});
  ::seated.push_back(plate);
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::cover() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) ::seated.clear();
  const CONTROL::Reel reel = CONTROL::reel();
  const Float opens = across(reel.at);
  const Float closes = std::max(opens, across(VIEWS::clock()));
  for (const String &plate : ::seated)
    GUI::set(page, plate.c_str(), GUI::Visibility{false});
  for (const ::Landing &fall : ::landings(reel)) {
    if (fall.band == NONE) continue;
    ::seat(page, fall);
    const String plate = ::id(fall);
    GUI::set(page, plate.c_str(), GUI::Visibility{true});
    GUI::set(page, plate.c_str(), GUI::Position{opens, inlay(fall.band)});
    GUI::set(
      page, plate.c_str(), GUI::Extent{closes - opens, inlaid(fall.band)});
  }
}

void SOUND::VIEWS::ARRANGEMENT::cover(SHELL::Session &session) {
  const CONTROL::Reel reel = CONTROL::reel();
  for (const ::Landing &fall : ::landings(reel)) {
    if (fall.band == NONE) continue;
    session.print(std::format(
      "cover {} lane {} band {} at {} for {}", KIND::spoken(fall.kind),
      fall.lane, fall.band, reel.at, reel.frames));
  }
}
