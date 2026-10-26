// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>
#include <island/gui/stroke.hpp>

#include "../../boards.hpp"
#include "lanes.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;

constexpr STRING::Hot DOOR = "arrangement.select";
constexpr STRING::Hot BAND = "arrangement.board.marquee";
constexpr STRING::Hot RINGED = "ring";
constexpr STRING::Hot MARKED = "selected";
constexpr STRING::Hot PLAIN = "icon";
constexpr Float HELD = 2.0f;

Flag selecting = false;
Flag stood = false;
Whole born = 0;

struct Reach {
  Float west = 0.0f, east = 0.0f, north = 0.0f, south = 0.0f;
};

auto reached(const GUI::SAC::STROKE::Gesture &gesture) -> ::Reach {
  return {
    std::min(gesture.from.at.x, gesture.to.at.x),
    std::max(gesture.from.at.x, gesture.to.at.x),
    std::min(gesture.from.at.y, gesture.to.at.y),
    std::max(gesture.from.at.y, gesture.to.at.y)};
}

auto caught(const ::Reach &band, const Box &box) -> Flag {
  const Float west = VIEWS::ARRANGEMENT::across(box.at);
  const Float east = west + VIEWS::ARRANGEMENT::across(box.frames);
  const Float north = VIEWS::ARRANGEMENT::inlay(box.band);
  const Float south = north + VIEWS::ARRANGEMENT::inlaid(box.band);
  return west < band.east && east > band.west && north < band.south &&
         south > band.north;
}

auto grounded(const GUI::SAC::STROKE::Gesture &gesture) -> Flag {
  return !VIEWS::ARRANGEMENT::gutter(gesture.from.at.x) &&
         !VIEWS::ARRANGEMENT::shelved(gesture.from.at.y) &&
         VIEWS::ARRANGEMENT::boxed(gesture.from.at.x, gesture.from.at.y) ==
           NONE;
}

void tools(GUI::Handle page) {
  if (GUI::GET::clicked(page, ::DOOR)) ::selecting = !::selecting;
  GUI::set(page, ::DOOR, GUI::Style{::selecting ? ::MARKED : ::PLAIN});
}

void shown(GUI::Handle page, const GUI::SAC::STROKE::Gesture &gesture) {
  const Flag showing = gesture.standing && gesture.dragged;
  GUI::set(page, ::BAND, GUI::Visibility{showing});
  if (!showing) return;
  const ::Reach band = ::reached(gesture);
  GUI::set(page, ::BAND, GUI::Position{band.west, band.north});
  GUI::set(
    page, ::BAND, GUI::Extent{band.east - band.west, band.south - band.north});
}

void closed(GUI::Handle page, const GUI::SAC::STROKE::Gesture &gesture) {
  const ::Reach band = ::reached(gesture);
  const Vector<Box> &laid = VIEWS::ARRANGEMENT::boxes();
  const String board = VIEWS::ARRANGEMENT::board();
  for (Whole row = 0; row < laid.size(); ++row)
    GUI::NGA::set(
      page, GUI::SAC::SEAT::named(board.c_str(), row).c_str(),
      GUI::NGA::Selected{
        gesture.dragged && !VIEWS::ARRANGEMENT::shut(laid[row].band) &&
        ::caught(band, laid[row])});
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::drawing(
  const GUI::SAC::STROKE::Gesture &gesture) -> Flag {
  return ::selecting == gesture.control;
}

void SOUND::VIEWS::ARRANGEMENT::easel() { ::tools(VIEWS::document()); }

void SOUND::VIEWS::ARRANGEMENT::easel(SHELL::Session &session) {
  if (::selecting) session.print("easel select");
  const Vector<Whole> rows = chosen();
  if (rows.empty()) return;
  String said;
  for (const Whole row : rows) said += std::format(" {}", row);
  session.print(std::format("chosen {}{}", rows.size(), said));
}

auto SOUND::VIEWS::ARRANGEMENT::chosen() -> Vector<Whole> {
  const String board = VIEWS::ARRANGEMENT::board();
  const Whole count = boxes().size();
  Vector<Whole> rows;
  for (const STRING::Cold &id :
       GUI::NGA::GET::selections(VIEWS::document(), board.c_str())) {
    const Whole row = GUI::SAC::SEAT::row(String(id), board.c_str());
    if (row < count && std::find(rows.begin(), rows.end(), row) == rows.end())
      rows.push_back(row);
  }
  std::sort(rows.begin(), rows.end());
  return rows;
}

void SOUND::VIEWS::ARRANGEMENT::chosen(const Vector<Whole> &rows) {
  const GUI::Handle page = VIEWS::document();
  const String board = VIEWS::ARRANGEMENT::board();
  for (Whole row = 0; row < boxes().size(); ++row)
    GUI::NGA::set(
      page, GUI::SAC::SEAT::named(board.c_str(), row).c_str(),
      GUI::NGA::Selected{
        std::find(rows.begin(), rows.end(), row) != rows.end()});
}

auto SOUND::VIEWS::ARRANGEMENT::mates(Whole row) -> Vector<Whole> {
  const Vector<Whole> rows = chosen();
  const Vector<Box> &laid = boxes();
  if (row >= laid.size()) return {};
  if (std::find(rows.begin(), rows.end(), row) == rows.end()) return {row};
  Vector<Whole> along;
  for (const Whole one : rows)
    if (laid[one].track == laid[row].track && laid[one].lane == laid[row].lane)
      along.push_back(one);
  return along;
}

void SOUND::VIEWS::ARRANGEMENT::marquee() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) ::stood = false;
  if (!::stood) {
    BOARDS::place(page, board().c_str(), "panel", String(::BAND), {}, {});
    GUI::set(page, ::BAND, GUI::Style{::RINGED});
    GUI::set(page, ::BAND, GUI::Depth{::HELD});
    ::stood = true;
  }
  const GUI::SAC::STROKE::Gesture gesture =
    GUI::SAC::STROKE::GET::gesture(page);
  const Flag picking = !drawing(gesture) && ::grounded(gesture);
  ::shown(
    page, gesture.standing && picking ? gesture : GUI::SAC::STROKE::Gesture{});
  if (gesture.closed && picking) ::closed(page, gesture);
}
