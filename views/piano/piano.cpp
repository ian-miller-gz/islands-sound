// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/carry.hpp>
#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NAME = "piano";
constexpr STRING::Hot RULER = "ruler";

Vector<VIEWS::ROLL::Pip> shown;
Flag parked = false;
Whole born = 0;

auto pips() -> Vector<VIEWS::ROLL::Pip> {
  Vector<VIEWS::ROLL::Pip> wanted;
  const Inventory &pool = INVENTORY::held();
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole row = 0; row < tracks.size(); ++row)
    for (const ARRANGEMENT::TRACK::Lane &lane : tracks[row].lanes) {
      if (lane.kind != KIND::NOTES) continue;
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips) {
        if (span.stock >= pool.stocks.size()) continue;
        const Score &score = pool.stocks[span.stock].score;
        const Whole opens = VIEWS::ROLL::opened(span.at);
        const Whole held = VIEWS::ROLL::opened(span.frames);
        for (Whole index = 0; index < score.notes.size(); ++index) {
          const Note &note = score.notes[index];
          if (note.at < span.from || note.at - span.from >= held) continue;
          wanted.push_back(
            {span.stock, index, note.pitch, opens + note.at - span.from,
             note.length, row});
        }
      }
    }
  return wanted;
}

constexpr STRING::Hot LOOSE = "loose";

auto loosely(const VIEWS::ROLL::Pip &pip) -> Flag { return pip.stock == NONE; }

void dressed(GUI::Handle page, const Vector<VIEWS::ROLL::Pip> &wanted) {
  const String board = VIEWS::ROLL::board();
  for (Whole row = 0; row < wanted.size(); ++row) {
    const Flag loose = ::loosely(wanted[row]);
    if (row < ::shown.size() && ::loosely(::shown[row]) == loose) continue;
    const String id = GUI::SAC::SEAT::named(board.c_str(), row);
    GUI::set(page, id.c_str(), GUI::Style{loose ? ::LOOSE : BOARDS::PLATE});
    GUI::set(
      page, id.c_str(),
      GUI::Depth{loose ? VIEWS::ROLL::LOOSE : VIEWS::ROLL::SEATED});
  }
}

void placed(GUI::Handle page, const Vector<VIEWS::ROLL::Pip> &wanted) {
  const String board = VIEWS::ROLL::board();
  for (Whole row = 0; row < wanted.size(); ++row) {
    const String id = GUI::SAC::SEAT::named(board.c_str(), row);
    if (!GUI::SAC::CARRY::GET::dragged(page, id.c_str()))
      GUI::set(
        page, id.c_str(),
        GUI::Position{
          VIEWS::ROLL::across(wanted[row].at),
          VIEWS::ROLL::lane(wanted[row].pitch)});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{VIEWS::ROLL::wide(wanted[row].length), VIEWS::ROLL::TALL});
  }
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
  Vector<VIEWS::ROLL::Pip> wanted = ::pips();
  VIEWS::ROLL::loosed(wanted);
  const String board = VIEWS::ROLL::board();
  BOARDS::sweep(page, board.c_str(), wanted.size(), ::shown.size());
  for (Whole row = ::shown.size(); row < wanted.size(); ++row)
    BOARDS::place(
      page, board.c_str(), "node", GUI::SAC::SEAT::named(board.c_str(), row),
      {}, {});
  VIEWS::ROLL::steadied(page, wanted);
  ::dressed(page, wanted);
  ::shown = wanted;
  ::placed(page, wanted);
  VIEWS::ROLL::walk(page);
}

auto marked() -> String {
  const Whole row =
    GUI::SAC::SEAT::selected(VIEWS::document(), VIEWS::ROLL::board().c_str());
  return row == NONE ? String("none") : std::to_string(row);
}

auto noted() -> Whole {
  Whole count = 0;
  for (const VIEWS::ROLL::Pip &pip : ::shown)
    if (!::loosely(pip)) ++count;
  return count;
}

void state(SHELL::Session &session) {
  session.print(std::format(
    "piano notes {} grain {} chosen {} pan {:g} zoom {:g}", ::noted(),
    VIEWS::ROLL::GRAIN, ::marked(),
    GUI::NGA::GET::pan(VIEWS::document(), VIEWS::ROLL::board().c_str()).x,
    VIEWS::ROLL::scaled()));
  const GUI::NGA::Zoom scale =
    GUI::NGA::GET::zoom(VIEWS::document(), VIEWS::ROLL::board().c_str());
  session.print(
    std::format("scale across {:g} down {:g}", scale.value, scale.down));
  VIEWS::ROLL::dials(session);
  VIEWS::ROLL::doors(session);
  VIEWS::ROLL::easel(session);
  VIEWS::ROLL::keys(session);
  VIEWS::ROLL::rail(session);
  VIEWS::ROLL::grid(session);
  VIEWS::ROLL::head(session);
  VIEWS::ROLL::ruler(session);
  VIEWS::ROLL::cycle(session);
  VIEWS::ROLL::range(session);
  VIEWS::ROLL::seams(session);
  for (Whole row = 0; row < ::shown.size(); ++row)
    if (::loosely(::shown[row]))
      session.print(std::format(
        "loose {} pitch {} at {} length {}", row, ::shown[row].pitch,
        ::shown[row].at, ::shown[row].length));
    else
      session.print(std::format(
        "note {} stock {} pitch {} at {} length {}", row, ::shown[row].stock,
        ::shown[row].pitch, ::shown[row].at, ::shown[row].length));
}

}  // namespace

auto SOUND::VIEWS::ROLL::face() -> const VIEWS::View & {
  static const View unit = {
    .name = ::NAME,
    .split = true,
    .least = least,
    .controls = {ZOOM, PAN},
    .run = ::run,
    .state = ::state,
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

auto SOUND::VIEWS::ROLL::plates() -> const Vector<Pip> & { return ::shown; }

auto SOUND::VIEWS::ROLL::wide(Whole length) -> Float {
  return std::max(across(length), LEAST / scaled());
}
