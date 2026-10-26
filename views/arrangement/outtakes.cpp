// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>
#include <island/gui/stroke.hpp>

#include "../../boards.hpp"
#include "../../timeline.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot BED = "kept";
constexpr STRING::Hot DRESSED = "outtake";
constexpr Float ORIGIN = 0.0f;

struct Plate {
  Whole track = 0, lane = 0, band = 0, out = 0;
  Whole at = 0, frames = 0;
};

Vector<Plate> shown;
Whole stood = 0;
Whole born = 0;

auto bed() -> String { return VIEWS::ARRANGEMENT::board() + "." + ::BED; }

auto framed(Float across) -> Whole {
  return across <= 0.0f ? 0 : Whole(across * Float(VIEWS::ARRANGEMENT::GRAIN));
}

auto plates() -> Vector<::Plate> {
  Vector<::Plate> wanted;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  Whole band = 0;
  for (Whole track = 0; track < tracks.size(); ++track)
    for (Whole lane = 0; lane < tracks[track].lanes.size(); ++lane, ++band) {
      const Vector<ARRANGEMENT::TRACK::LANE::Clip> &kept =
        tracks[track].lanes[lane].outtakes;
      for (Whole out = 0; out < kept.size(); ++out)
        wanted.push_back(
          {track, lane, band, out, kept[out].at, kept[out].frames});
    }
  return wanted;
}

void build(GUI::Handle page, Whole count) {
  const String ground = ::bed();
  if (::stood == 0 && count != 0)
    BOARDS::place(
      page, VIEWS::ARRANGEMENT::board().c_str(), "panel", ground, {}, {});
  BOARDS::sweep(page, ground.c_str(), count, ::stood);
  for (Whole plate = ::stood; plate < count; ++plate) {
    const String id = GUI::SAC::SEAT::named(ground.c_str(), plate);
    BOARDS::place(page, ground.c_str(), "node", id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::DRESSED});
    GUI::set(page, id.c_str(), GUI::Depth{VIEWS::ARRANGEMENT::RAISED});
  }
  ::stood = count;
}

auto pressed(Float across, Float down) -> Whole {
  if (!VIEWS::ARRANGEMENT::shelved(down)) return NONE;
  const Whole at = ::framed(across);
  for (Whole plate = 0; plate < ::shown.size(); ++plate) {
    const ::Plate &held = ::shown[plate];
    const Float top = VIEWS::ARRANGEMENT::rail(held.band) +
                      VIEWS::ARRANGEMENT::depth(held.band) -
                      VIEWS::ARRANGEMENT::shelf(held.band);
    if (down < top) continue;
    if (down >= top + VIEWS::ARRANGEMENT::SHELF) continue;
    if (at >= held.at && at <= held.at + held.frames) return plate;
  }
  return NONE;
}

auto runged(Whole at) -> Whole {
  const Whole step = VIEWS::ARRANGEMENT::rung().span;
  return step == 0 ? at : (at + step / 2) / step * step;
}

auto landed(const ::Plate &plate, const GUI::SAC::STROKE::Gesture &gesture)
  -> Whole {
  const Whole press = ::framed(gesture.from.at.x);
  const Whole to = ::framed(gesture.to.at.x);
  return to >= press             ? plate.at + (to - press)
         : plate.at > press - to ? plate.at - (press - to)
                                 : 0;
}

void lifted(const GUI::SAC::STROKE::Gesture &gesture) {
  const Whole plate = ::pressed(gesture.from.at.x, gesture.from.at.y);
  if (plate == NONE) return;
  const ::Plate &held = ::shown[plate];
  const Whole at = ::landed(held, gesture);
  TIMELINE::restore(
    held.track, held.lane, held.out,
    VIEWS::ARRANGEMENT::snapped() ? ::runged(at) : at);
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::outtakes() {
  const GUI::Handle page = VIEWS::document();
  const GUI::SAC::STROKE::Gesture gesture =
    GUI::SAC::STROKE::GET::gesture(page);
  if (gesture.closed) ::lifted(gesture);
  if (VIEWS::reborn(::born)) ::stood = 0;
  const Vector<::Plate> wanted = ::plates();
  if (wanted.size() != ::stood) ::build(page, wanted.size());
  ::shown = wanted;
  for (Whole plate = 0; plate < ::shown.size(); ++plate) {
    const String id = GUI::SAC::SEAT::named(::bed().c_str(), plate);
    const ::Plate &held = ::shown[plate];
    Float across = ARRANGEMENT::across(held.at);
    if (
      gesture.standing &&
      ::pressed(gesture.from.at.x, gesture.from.at.y) == plate)
      across = ARRANGEMENT::across(::landed(held, gesture));
    const Float top = rail(held.band) + depth(held.band) - shelf(held.band);
    GUI::set(page, id.c_str(), GUI::Position{across, top + INLAY});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{
        ARRANGEMENT::across(held.frames), SHELF - INLAY * 2.0f - CURB});
  }
}

void SOUND::VIEWS::ARRANGEMENT::outtakes(SHELL::Session &session) {
  session.print(std::format("kept {}", ::shown.size()));
  for (Whole plate = 0; plate < ::shown.size(); ++plate)
    session.print(std::format(
      "kept {} track {} lane {} at {} for {}", plate, ::shown[plate].track,
      ::shown[plate].lane, ::shown[plate].at, ::shown[plate].frames));
}
