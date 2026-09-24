// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../kind.hpp"
#include "../../shape.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NODE = "node";
constexpr STRING::Hot PLATED = "plate";

Vector<VIEWS::SIGNALS::Point> laid;
VIEWS::Pool pool;

auto seat(Whole row) -> String {
  return GUI::SAC::SEAT::named(VIEWS::SIGNALS::board().c_str(), row);
}

void build(GUI::Handle page, const String &field, Whole count) {
  BOARDS::sweep(page, field.c_str(), count, ::pool.stood);
  for (Whole row = ::pool.stood; row < count; ++row) {
    const String id = ::seat(row);
    BOARDS::place(page, field.c_str(), ::NODE, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Depth{VIEWS::SIGNALS::SEATED});
  }
  VIEWS::built(::pool, count);
}

void placed(GUI::Handle page, const String &field) {
  const String skin = VIEWS::SIGNALS::dressed(::PLATED);
  const Float wide = VIEWS::SIGNALS::wide();
  const Float deep = VIEWS::SIGNALS::deep();
  for (Whole row = 0; row < ::laid.size(); ++row) {
    const String id = ::seat(row);
    GUI::set(page, id.c_str(), GUI::Style{skin.c_str()});
    if (GUI::SAC::CARRY::GET::dragged(page, id.c_str())) continue;
    GUI::set(
      page, id.c_str(),
      GUI::Position{
        VIEWS::SIGNALS::across(::laid[row].at) - wide * VIEWS::SIGNALS::HALF,
        VIEWS::SIGNALS::down(::laid[row].value) - deep * VIEWS::SIGNALS::HALF});
    GUI::set(page, id.c_str(), GUI::Extent{wide, deep});
  }
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::dressed(STRING::Hot stem) -> String {
  return String(stem) + KIND::spoken(paged());
}

auto SOUND::VIEWS::SIGNALS::wide() -> Float {
  const Float scale = scaled();
  return scale > 0.0f ? PLATE / scale : PLATE;
}

auto SOUND::VIEWS::SIGNALS::deep() -> Float {
  const Float scale = sunk();
  return scale > 0.0f ? PLATE / scale : PLATE;
}

void SOUND::VIEWS::SIGNALS::plates() {
  const GUI::Handle page = document();
  const String field = board();
  if (VIEWS::reborn(::pool)) ::laid.clear();
  Vector<Point> wanted = points();
  steadied(wanted);
  const Whole seats = VIEWS::sized(::pool, wanted.size(), window());
  if (seats != ::pool.stood) ::build(page, field, seats);
  VIEWS::light(::pool, ::seat, wanted.size());
  followed(page, wanted);
  ::laid = wanted;
  ::placed(page, field);
  segments();
}

void SOUND::VIEWS::SIGNALS::SHED::plates() {
  VIEWS::reborn(::pool);
  ::build(document(), board(), 0);
  ::laid.clear();
}

auto SOUND::VIEWS::SIGNALS::plated() -> Whole { return ::pool.stood; }

auto SOUND::VIEWS::SIGNALS::shown() -> const Vector<Point> & { return ::laid; }

void SOUND::VIEWS::SIGNALS::plates(SHELL::Session &session) {
  for (Whole row = 0; row < ::laid.size(); ++row)
    session.print(std::format(
      "plate {} stock {} index {} at {} value {:g} until {} shape {}", row,
      ::laid[row].stock, ::laid[row].index, ::laid[row].at, ::laid[row].value,
      ::laid[row].until, SHAPE::spoken(::laid[row].shape)));
}
