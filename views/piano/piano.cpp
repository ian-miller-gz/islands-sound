// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NAME = "piano";
constexpr STRING::Hot RULER = "ruler";

Vector<VIEWS::ROLL::Pip> shown;
Flag parked = false;
Whole born = 0;

void tools() {
  VIEWS::ROLL::ruler();
  VIEWS::ROLL::dials();
  VIEWS::ROLL::doors();
  VIEWS::ROLL::hands();
  VIEWS::ROLL::keyed();
  VIEWS::ROLL::rail();
  VIEWS::ROLL::easel();
  VIEWS::ROLL::keys();
  VIEWS::ROLL::grid();
  VIEWS::ROLL::range();
  VIEWS::ROLL::seams();
  VIEWS::ROLL::head();
}

void plates(GUI::Handle page) {
  Vector<VIEWS::ROLL::Pip> wanted = VIEWS::ROLL::pips();
  VIEWS::ROLL::loosed(wanted);
  const String board = VIEWS::ROLL::board();
  BOARDS::sweep(page, board.c_str(), wanted.size(), ::shown.size());
  for (Whole row = ::shown.size(); row < wanted.size(); ++row)
    BOARDS::place(
      page, board.c_str(), "node", GUI::SAC::SEAT::named(board.c_str(), row),
      {}, {});
  VIEWS::ROLL::steadied(page, wanted);
  VIEWS::ROLL::seat(page, wanted, ::shown);
  ::shown = wanted;
}

void run() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) {
    ::shown.clear();
    ::parked = false;
  }
  VIEWS::ROLL::bound();
  VIEWS::ROLL::chase();
  if (!::parked) {
    VIEWS::ROLL::park();
    ::parked = true;
  }
  ::tools();
  ::plates(page);
  VIEWS::ROLL::walk(page);
}

}  // namespace

auto SOUND::VIEWS::ROLL::face() -> const VIEWS::View& {
  static const View unit = {
    .name = ::NAME,
    .split = true,
    .least = least,
    .controls = {ZOOM, PAN},
    .run = ::run,
    .state = state,
    .overview = overview,
    .shed = shed};
  return unit;
}

void SOUND::VIEWS::ROLL::SHED::plates() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) ::shown.clear();
  BOARDS::sweep(page, board().c_str(), 0, ::shown.size());
  ::shown.clear();
}

auto SOUND::VIEWS::ROLL::board() -> String {
  return String(::NAME) + "." + BOARD;
}

auto SOUND::VIEWS::ROLL::scaled() -> Float {
  return GUI::NGA::GET::zoom(document(), board().c_str()).value;
}

auto SOUND::VIEWS::ROLL::strip() -> String {
  return String(::NAME) + "." + ::RULER;
}

auto SOUND::VIEWS::ROLL::plates() -> const Vector<Pip>& { return ::shown; }
