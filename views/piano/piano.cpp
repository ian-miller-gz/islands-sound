// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "piano.internal.hpp"
#include "piano.loudness.hpp"
#include "piano.notation.hpp"
#include "piano.paper.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NAME = "piano";

Vector<VIEWS::ROLL::Pip> shown;
Flag parked = false;
Whole born = 0;

void tools() {
  VIEWS::ROLL::ROWS::rows();
  VIEWS::ROLL::STAFF::drawn();
  VIEWS::ROLL::ruler();
  VIEWS::ROLL::dials();
  VIEWS::ROLL::doors();
  VIEWS::ROLL::hands();
  VIEWS::ROLL::keyed();
  VIEWS::ROLL::rail();
  VIEWS::ROLL::plus();
  VIEWS::ROLL::easel();
  VIEWS::ROLL::keys();
  VIEWS::ROLL::grid();
  VIEWS::ROLL::range();
  VIEWS::ROLL::STAFF::clef();
  VIEWS::ROLL::seams();
  VIEWS::ROLL::ends();
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
  // The plates stand before the stems are read off them, so a note
  // authored between two frames is stemmed in the frame that shows it;
  // the heads then seat with this frame's stems (their flips read them).
  const Vector<VIEWS::ROLL::Pip> was = std::move(::shown);
  ::shown = wanted;
  VIEWS::ROLL::STAFF::stemmed();
  VIEWS::ROLL::steadied(page, wanted);
  VIEWS::ROLL::seat(page, wanted, was);
}

void run() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) {
    ::shown.clear();
    ::parked = VIEWS::LANE::GET::seen();
  }
  VIEWS::ROLL::bound();
  VIEWS::ROLL::chase();
  VIEWS::ROLL::settle();
  if (!::parked) {
    VIEWS::ROLL::park();
    ::parked = true;
  }
  ::tools();
  ::plates(page);
  VIEWS::ROLL::loudness();
  VIEWS::ROLL::walk(page);
}

}  // namespace

auto SOUND::VIEWS::ROLL::face() -> const VIEWS::View& {
  static const View unit = {
    .name = ::NAME,
    .split = true,
    .least = least,
    .most = most,
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

auto SOUND::VIEWS::ROLL::plates() -> const Vector<Pip>& { return ::shown; }
