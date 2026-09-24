// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../kind.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NAME = "signals";
constexpr STRING::Hot RULER = "ruler";
constexpr STRING::Hot RAIL = "rail";

void run() {
  VIEWS::SIGNALS::hands();
  VIEWS::SIGNALS::keyed();
  VIEWS::SIGNALS::menu();
  VIEWS::SIGNALS::bound();
  VIEWS::SIGNALS::chase();
  VIEWS::SIGNALS::pager();
  VIEWS::SIGNALS::mapping();
  VIEWS::SIGNALS::numbers();
  VIEWS::SIGNALS::bench();
  VIEWS::SIGNALS::easel();
  VIEWS::SIGNALS::ruler();
  VIEWS::SIGNALS::rail();
  VIEWS::SIGNALS::grid();
  VIEWS::SIGNALS::range();
  VIEWS::SIGNALS::head();
  VIEWS::SIGNALS::plates();
}

void state(SHELL::Session &session) {
  session.print(std::format(
    "signals plates {} page {} kind {}", VIEWS::SIGNALS::shown().size(),
    VIEWS::SIGNALS::number() == NONE ? String("none")
                                     : std::to_string(VIEWS::SIGNALS::number()),
    KIND::spoken(VIEWS::SIGNALS::paged())));
  session.print(std::format(
    "scale across {:g} down {:g}", VIEWS::SIGNALS::scaled(),
    VIEWS::SIGNALS::sunk()));
  VIEWS::SIGNALS::pager(session);
  VIEWS::SIGNALS::mapping(session);
  VIEWS::SIGNALS::numbers(session);
  VIEWS::SIGNALS::bench(session);
  VIEWS::SIGNALS::easel(session);
  VIEWS::SIGNALS::aimed(session);
  VIEWS::SIGNALS::ruler(session);
  VIEWS::SIGNALS::rail(session);
  VIEWS::SIGNALS::grid(session);
  VIEWS::SIGNALS::range(session);
  VIEWS::SIGNALS::head(session);
  VIEWS::SIGNALS::plates(session);
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::face() -> const VIEWS::View & {
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

auto SOUND::VIEWS::SIGNALS::board() -> String {
  return String(::NAME) + "." + BOARD;
}

auto SOUND::VIEWS::SIGNALS::strip() -> String {
  return String(::NAME) + "." + ::RULER;
}

auto SOUND::VIEWS::SIGNALS::column() -> String {
  return String(::NAME) + "." + ::RAIL;
}

auto SOUND::VIEWS::SIGNALS::scaled() -> Float {
  return GUI::NGA::GET::zoom(document(), board().c_str()).value;
}

auto SOUND::VIEWS::SIGNALS::sunk() -> Float {
  return GUI::NGA::GET::zoom(document(), board().c_str()).down;
}
