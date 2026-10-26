// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace ROWS = SOUND::VIEWS::ROLL::ROWS;

constexpr STRING::Hot STYLE = "plus";
constexpr STRING::Hot GLYPH = "+";
constexpr Float MARK = 10.0f;
constexpr Float SIDE = 12.0f;
constexpr Float OVER = 1.0f;

struct Seated {
  Key key;
  Vector<ROWS::Gap> gaps;
  Float stand = ROWS::OFF;
};

Vector<Seated> seated;
Vector<Key> worn;
Flag folded = false;
Whole born = 0;
Whole rowed = 0;

void reset() {
  ::seated.clear();
  ::worn.clear();
  ::folded = false;
}

void seat(GUI::Handle page, Whole section, const ROWS::Gap &gap) {
  const String name = ROLL::PLUS::sectioned(section, gap);
  BOARDS::place(
    page, ROLL::board().c_str(), "button", name, {}, {::SIDE, ::SIDE});
  GUI::set(page, name.c_str(), GUI::Depth{ROLL::RAISED + ::OVER});
  GUI::NGA::set(page, name.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::ACROSS});
  GUI::set(page, name.c_str(), GUI::Text{::GLYPH});
  GUI::set(page, name.c_str(), GUI::Size{::MARK});
  GUI::set(page, name.c_str(), GUI::Style{::STYLE});
}

void strike(GUI::Handle page) {
  for (Whole section = 0; section < ::seated.size(); ++section)
    for (const ROWS::Gap &gap : ::seated[section].gaps)
      BOARDS::drop(page, ROLL::PLUS::sectioned(section, gap));
  ::seated.clear();
}

void relaid(GUI::Handle page) {
  ::strike(page);
  if (!::folded) return;
  for (const ROLL::Keyed &laid : ROLL::keyed({})) {
    ::seated.push_back({.key = laid.key, .gaps = ROWS::gaps(laid.key)});
    for (const ROWS::Gap &gap : ::seated.back().gaps)
      ::seat(page, ::seated.size() - 1, gap);
  }
}

void stand(GUI::Handle page, Whole section) {
  Seated &one = ::seated[section];
  one.stand = ROLL::stood(section);
  const Float x = one.stand + ROLL::RAILED - ::SIDE;
  for (const ROWS::Gap &gap : one.gaps) {
    const String name = ROLL::PLUS::sectioned(section, gap);
    const Float line = ROWS::lane(gap.high + 1, one.key.at) + ROLL::STEP;
    GUI::set(page, name.c_str(), GUI::Visibility{one.stand > -ROLL::RAILED});
    GUI::set(page, name.c_str(), GUI::Position{x, line - ::SIDE * ROLL::HALF});
  }
}

}  // namespace

void SOUND::VIEWS::ROLL::plus() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::reset();
  Flag moved = ROWS::moved(::rowed);
  if (rekeyed(::worn) || ::folded != ROWS::folded()) {
    ::folded = ROWS::folded();
    ::relaid(page);
    moved = true;
  }
  for (Whole section = 0; section < ::seated.size(); ++section) {
    if (moved || stood(section) != ::seated[section].stand)
      ::stand(page, section);
    PLUS::pressed(page, section, ::seated[section].key, ::seated[section].gaps);
  }
  PLUS::taken();
}

void SOUND::VIEWS::ROLL::SHED::plus() {
  const GUI::Handle page = document();
  if (!VIEWS::reborn(::born)) ::strike(page);
  ::reset();
}
