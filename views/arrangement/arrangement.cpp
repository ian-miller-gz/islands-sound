// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "arrangement.face.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NAME = "arrangement";
constexpr STRING::Hot RULER = "ruler";
constexpr STRING::Hot MAPPED = "minimap";
constexpr Float LETTER = 15.0f;
constexpr STRING::Hot PLATED = "plate";
constexpr STRING::Hot LABELED = "label";
constexpr STRING::Hot INKED = "boxlabel";
constexpr Float INSET = VIEWS::ARRANGEMENT::GRIP + VIEWS::ARRANGEMENT::INLAY;
constexpr Float PAD = 4.0f;
constexpr Float NAMED = 160.0f;
constexpr Float ORIGIN = 0.0f;

constexpr Float START = 0.2f;

constexpr Float LEVEL = 1.0f;

using VIEWS::ARRANGEMENT::Box;

Vector<Box> shown;
Whole born = 0;
Flag parked = false;

auto laid() -> Vector<Box> {
  Vector<Box> wanted;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  Whole band = 0;
  for (Whole track = 0; track < tracks.size(); ++track)
    for (Whole lane = 0; lane < tracks[track].lanes.size(); ++lane, ++band)
      for (Whole placement = 0;
           placement < tracks[track].lanes[lane].clips.size(); ++placement) {
        const ARRANGEMENT::TRACK::LANE::Clip &span =
          tracks[track].lanes[lane].clips[placement];
        wanted.push_back(
          {track, lane, band, tracks[track].lanes[lane].kind, placement,
           span.stock, span.at, span.from, span.frames});
      }
  return wanted;
}

auto named(const Box &box) -> String {
  const Inventory &pool = INVENTORY::held();
  return box.stock < pool.stocks.size() ? pool.stocks[box.stock].name
                                        : String("?");
}

void bound() {
  const Float scale = ::parked ? VIEWS::ARRANGEMENT::scaled() : ::START;
  GUI::NGA::set(
    VIEWS::document(), VIEWS::ARRANGEMENT::board().c_str(),
    GUI::NGA::Bounds{
      -VIEWS::ARRANGEMENT::GUTTER / scale, GUI::NGA::Bounds::NONE, ::ORIGIN,
      VIEWS::ARRANGEMENT::deep()});
}

void park() {
  GUI::NGA::set(
    VIEWS::document(), VIEWS::ARRANGEMENT::board().c_str(),
    GUI::NGA::Zoom{::START, ::LEVEL});
  GUI::NGA::set(
    VIEWS::document(), VIEWS::ARRANGEMENT::board().c_str(),
    GUI::NGA::Pan{-VIEWS::ARRANGEMENT::GUTTER / ::START, ::ORIGIN});
}

void run() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) {
    ::shown.clear();
    ::parked = false;
  }
  ::bound();
  if (!::parked) {
    ::park();
    ::parked = true;
  }
  VIEWS::ARRANGEMENT::chase();
  VIEWS::ARRANGEMENT::bands();
  VIEWS::ARRANGEMENT::names();
  VIEWS::ARRANGEMENT::ruler();
  VIEWS::ARRANGEMENT::minimap();
  VIEWS::ARRANGEMENT::head();
  VIEWS::ARRANGEMENT::cover();
  VIEWS::ARRANGEMENT::snap();
  VIEWS::ARRANGEMENT::menu();
  VIEWS::ARRANGEMENT::keys();
  VIEWS::ARRANGEMENT::choose();
  VIEWS::ARRANGEMENT::easel();
  VIEWS::ARRANGEMENT::grips();
  VIEWS::ARRANGEMENT::draft();
  VIEWS::ARRANGEMENT::marquee();
  const Vector<Box> wanted = ::laid();
  BOARDS::sweep(
    page, VIEWS::ARRANGEMENT::board().c_str(), wanted.size(), ::shown.size());
  for (Whole row = ::shown.size(); row < wanted.size(); ++row) {
    const String id =
      GUI::SAC::SEAT::named(VIEWS::ARRANGEMENT::board().c_str(), row);
    BOARDS::place(
      page, VIEWS::ARRANGEMENT::board().c_str(), "node", id, {}, {});
    GUI::set(page, id.c_str(), GUI::Clipping{true});
    GUI::set(page, id.c_str(), GUI::Depth{VIEWS::ARRANGEMENT::RAISED});
    const String label = id + "." + ::LABELED;
    BOARDS::place(
      page, id.c_str(), "label", label, {::INSET, ::PAD}, {::NAMED, ::LETTER});
    GUI::set(page, label.c_str(), GUI::Style{::INKED});
    GUI::set(page, label.c_str(), GUI::Size{::LETTER});
    GUI::set(page, label.c_str(), GUI::Depth{VIEWS::ARRANGEMENT::RAISED});
    GUI::NGA::set(
      page, label.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::ACROSS});
  }
  ::shown = wanted;
  for (Whole row = 0; row < wanted.size(); ++row) {
    const String id =
      GUI::SAC::SEAT::named(VIEWS::ARRANGEMENT::board().c_str(), row);
    const Box laid = VIEWS::ARRANGEMENT::drawn(row);
    GUI::set(
      page, id.c_str(),
      GUI::Position{
        VIEWS::ARRANGEMENT::across(laid.at),
        VIEWS::ARRANGEMENT::inlay(laid.band)});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{
        VIEWS::ARRANGEMENT::across(laid.frames),
        VIEWS::ARRANGEMENT::inlaid(laid.band)});
    GUI::set(
      page, id.c_str(),
      GUI::Style{VIEWS::ARRANGEMENT::plated(wanted[row].kind).c_str()});
    GUI::set(
      page, (id + "." + ::LABELED).c_str(), GUI::Text{::named(wanted[row])});
  }
  VIEWS::ARRANGEMENT::walk();
  VIEWS::ARRANGEMENT::sketch();
  VIEWS::ARRANGEMENT::notches();
  VIEWS::ARRANGEMENT::ends();
  VIEWS::ARRANGEMENT::outtakes();
  VIEWS::ARRANGEMENT::ghost();
}

void state(SHELL::Session &session) {
  const String board = VIEWS::ARRANGEMENT::board();
  session.print(std::format(
    "arrangement boxes {} playhead {} grain {} pan {} zoom {}", ::shown.size(),
    TRANSPORT::marker().position, VIEWS::ARRANGEMENT::GRAIN,
    GUI::NGA::GET::pan(VIEWS::document(), board.c_str()).x,
    GUI::NGA::GET::zoom(VIEWS::document(), board.c_str()).value));
  VIEWS::ARRANGEMENT::snap(session);
  VIEWS::ARRANGEMENT::easel(session);
  VIEWS::ARRANGEMENT::ruler(session);
  VIEWS::ARRANGEMENT::cycle(session);
  VIEWS::ARRANGEMENT::minimap(session);
  VIEWS::ARRANGEMENT::bands(session);
  VIEWS::ARRANGEMENT::notches(session);
  VIEWS::ARRANGEMENT::outtakes(session);
  VIEWS::ARRANGEMENT::cover(session);
  VIEWS::ARRANGEMENT::sketch(session);
  for (Whole row = 0; row < ::shown.size(); ++row)
    session.print(std::format(
      "box {} track {} lane {} band {} stock {} at {} for {} wears {}", row,
      ::shown[row].track, ::shown[row].lane, ::shown[row].band,
      ::named(::shown[row]), ::shown[row].at, ::shown[row].frames,
      VIEWS::ARRANGEMENT::plated(::shown[row].kind)));
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::plated(Whole kind) -> String {
  return String(::PLATED) + KIND::spoken(kind);
}

auto SOUND::VIEWS::ARRANGEMENT::face() -> const VIEWS::View & {
  static const View unit = {
    .name = ::NAME,
    .flat = true,
    .least = least,
    .controls = {ZOOM, PAN},
    .run = ::run,
    .state = ::state,
    .shed = shed};
  return unit;
}

auto SOUND::VIEWS::ARRANGEMENT::board() -> String {
  return String(::NAME) + "." + BOARD;
}

auto SOUND::VIEWS::ARRANGEMENT::boxes() -> const Vector<Box> & {
  return ::shown;
}

auto SOUND::VIEWS::ARRANGEMENT::scaled() -> Float {
  return GUI::NGA::GET::zoom(VIEWS::document(), board().c_str()).value;
}

auto SOUND::VIEWS::ARRANGEMENT::strip() -> String {
  return String(::NAME) + "." + ::RULER;
}

auto SOUND::VIEWS::ARRANGEMENT::map() -> String {
  return String(::NAME) + "." + ::MAPPED;
}
