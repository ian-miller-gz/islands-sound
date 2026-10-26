// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../views.menu.marked.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace ROWS = SOUND::VIEWS::ROLL::ROWS;

constexpr STRING::Hot WHOSE = "piano.plus";

Key raised;
ROWS::Gap opened;
String door;

void raise(const String &name, const Key &key, const ROWS::Gap &gap) {
  Vector<String> rows;
  Vector<Flag> ticks;
  for (Whole pitch = gap.high + 1; pitch-- > gap.low;) {
    rows.push_back(ROWS::named(pitch));
    ticks.push_back(ROWS::lane(pitch, key.at) != ROWS::OFF);
  }
  ::raised = key;
  ::opened = gap;
  ::door = name;
  VIEWS::MENU::raise(rows, ::WHOSE, ::door.c_str(), ticks);
}

void press(const String &name, const Key &key, const ROWS::Gap &gap) {
  if (gap.low == gap.high) return ROWS::add(key, gap.low);
  ::raise(name, key, gap);
}

}  // namespace

auto SOUND::VIEWS::ROLL::PLUS::sectioned(Whole section, const ROWS::Gap &gap)
  -> String {
  return ROLL::sectioned(STEM, section, gap.low);
}

void SOUND::VIEWS::ROLL::PLUS::pressed(
  GUI::Handle page, Whole section, const Key &key,
  const Vector<ROWS::Gap> &gaps) {
  for (const ROWS::Gap &gap : gaps) {
    const String name = sectioned(section, gap);
    if (GUI::GET::clicked(page, name.c_str())) return ::press(name, key, gap);
  }
}

void SOUND::VIEWS::ROLL::PLUS::taken() {
  const Whole row = MENU::taken(::WHOSE);
  if (row <= ::opened.high - ::opened.low)
    ROWS::add(::raised, ::opened.high - row);
}
