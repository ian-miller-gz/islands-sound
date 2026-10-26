// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../commands.hpp"
#include "../../history.hpp"
#include "../../timeline.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LIFTS = "Delete";
constexpr STRING::Hot JOINS = "Append";
constexpr Whole JOIN = 1;
constexpr STRING::Hot TRACKED = "Track";
constexpr STRING::Hot BUSSED = "Bus";
constexpr Whole BUS = 1;

struct Subject {
  Whole track = NONE, lane = 0, placement = NONE;
  Vector<Whole> pieces;
};
Subject about;

auto pieces(const VIEWS::ARRANGEMENT::Box &box) -> Vector<Whole> {
  const Vector<VIEWS::ARRANGEMENT::Box> &laid = VIEWS::ARRANGEMENT::boxes();
  Vector<Whole> rows;
  for (const Whole row : VIEWS::ARRANGEMENT::chosen())
    if (laid[row].track == box.track && laid[row].lane == box.lane)
      rows.push_back(laid[row].placement);
  return rows;
}

auto aboard(GUI::Handle page, const GUI::NGA::Ask &ask) -> GUI::Position {
  const String id = VIEWS::ARRANGEMENT::board();
  const GUI::Position corner = GUI::GET::origin(page, id.c_str());
  const GUI::NGA::Pan pan = GUI::NGA::GET::pan(page, id.c_str());
  const GUI::NGA::Zoom zoom = GUI::NGA::GET::zoom(page, id.c_str());
  if (zoom.value <= 0.0f) return {ask.x, ask.y};
  const Float down = zoom.down > 0.0f ? zoom.down : zoom.value;
  return {
    pan.x + (ask.x - corner.x) / zoom.value, pan.y + (ask.y - corner.y) / down};
}

void took(Whole row) {
  const ::Subject held = ::about;
  ::about = {};
  if (held.placement == NONE)
    return void(COMMANDS::bussed(held.track, row == ::BUS));
  if (row == ::JOIN)
    return void(COMMANDS::joined(held.track, held.lane, held.pieces));
  VIEWS::ARRANGEMENT::lift(held.track, held.lane, held.placement);
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::lift(Whole track, Whole lane, Whole placement)
  -> Flag {
  const Vector<SOUND::ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (track >= tracks.size() || lane >= tracks[track].lanes.size())
    return false;
  const auto &clips = tracks[track].lanes[lane].clips;
  if (placement >= clips.size()) return false;
  const Placement covered = {track, lane, placement, clips[placement]};
  if (!TIMELINE::lift(track, lane, placement)) return false;
  HISTORY::record(
    {.act = "lift", .verb = Edit::REMOVED, .placements = {covered}});
  return true;
}

auto SOUND::VIEWS::ARRANGEMENT::lifted(const Vector<Whole> &rows) -> Flag {
  const Vector<Box> &laid = boxes();
  Vector<Placement> covered;
  for (const Whole row : rows) {
    if (row >= laid.size()) continue;
    const Box &box = laid[row];
    const auto &clips =
      TIMELINE::held().tracks[box.track].lanes[box.lane].clips;
    if (box.placement < clips.size())
      covered.push_back(
        {box.track, box.lane, box.placement, clips[box.placement]});
  }
  if (covered.empty()) return false;
  for (Whole back = covered.size(); back > 0; --back) {
    const Placement &one = covered[back - 1];
    if (!TIMELINE::lift(one.track, one.lane, one.placement)) return false;
  }
  HISTORY::record(
    {.act = "lift", .verb = Edit::REMOVED, .placements = covered});
  chosen({});
  return true;
}

void SOUND::VIEWS::ARRANGEMENT::menu() {
  const GUI::Handle page = VIEWS::document();
  if (::about.track != NONE) {
    const Whole row = VIEWS::MENU::taken("arrangement");
    if (row != NONE) return ::took(row);
    if (!VIEWS::MENU::standing()) ::about = {};
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(page);
  if (ask.board.empty() || ask.board != board()) return;
  const GUI::Position at = ::aboard(page, ask);
  const Whole box = boxed(at.x, at.y);
  if (box != NONE) {
    const Box &held = boxes()[box];
    ::about = {held.track, held.lane, held.placement, ::pieces(held)};
    Vector<String> rows = {String(::LIFTS)};
    if (::about.pieces.size() >= 2) rows.push_back(String(::JOINS));
    return VIEWS::MENU::raise(rows, ask.x, ask.y, "arrangement");
  }
  const Whole track = ribboned(at.y);
  if (track == NONE) return;
  ::about = {.track = track};
  VIEWS::MENU::raise(
    {String(::TRACKED), String(::BUSSED)}, ask.x, ask.y, "arrangement");
}
