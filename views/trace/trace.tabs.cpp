// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iterator>
#include <span>

#include "trace.internal.hpp"

namespace {
using namespace SOUND;
namespace TRACE = SOUND::VIEWS::TRACE;

constexpr STRING::Hot RESTING = "tab";
constexpr STRING::Hot STANDING = "standing";
constexpr STRING::Hot RESTINGWORD = "tabword";
constexpr STRING::Hot STANDINGWORD = "standingword";
constexpr STRING::Hot PLATE = "plate";
constexpr STRING::Hot FOOT = "trace.face.foot";
constexpr STRING::Hot UNCHOSEN = "no track chosen";
constexpr STRING::Hot UNCLIPPED = "no clips";
constexpr Float FOOTED = 1.0f;

constexpr STRING::Hot LANING[] = {TRACE::KINDED, TRACE::GROW};

struct Tab {
  STRING::Hot id = "";
  STRING::Hot list = "";
  void (*read)() = nullptr;
  GUI::Anchor anchor = GUI::NORTHWEST;
  STRING::Hot empty = "";
  std::span<const STRING::Hot> furniture;
};

constexpr Tab TABS[] = {
  {TRACE::NODED, TRACE::ROWS, TRACE::nodes, GUI::NORTHWEST, ::UNCHOSEN},
  {TRACE::LANED, TRACE::LANES, TRACE::lanes, GUI::NORTHWEST, ::UNCHOSEN,
   ::LANING},
  {TRACE::CLIPPED, TRACE::CLIPS, TRACE::clips, GUI::NORTHEAST, ::UNCLIPPED},
};

Whole faced = 0;

void dressed(GUI::Handle page, const Tab &tab, Flag standing) {
  const String plate = String(tab.id) + "." + ::PLATE;
  GUI::set(page, plate.c_str(), GUI::Style{standing ? ::STANDING : ::RESTING});
  GUI::set(page, tab.id, GUI::Style{standing ? ::STANDINGWORD : ::RESTINGWORD});
  GUI::set(page, tab.list, GUI::Visibility{standing});
  for (STRING::Hot id : tab.furniture)
    GUI::set(page, id, GUI::Visibility{standing});
}

void footed(GUI::Handle page, const Tab &tab) {
  const String plate = String(tab.id) + "." + ::PLATE;
  const Flag east = tab.anchor == GUI::NORTHEAST;
  GUI::Position foot = GUI::GET::position(page, ::FOOT);
  foot.x =
    GUI::GET::position(page, plate.c_str()).x + (east ? -::FOOTED : ::FOOTED);
  GUI::set(page, ::FOOT, tab.anchor);
  GUI::set(page, ::FOOT, foot);
}

}  // namespace

void SOUND::VIEWS::TRACE::turned(GUI::Handle page) {
  for (Whole row = 0; row < std::size(::TABS); ++row)
    if (GUI::GET::clicked(page, ::TABS[row].id)) ::faced = row;
  for (Whole row = 0; row < std::size(::TABS); ++row)
    ::dressed(page, ::TABS[row], row == ::faced);
  const Tab &tab = ::TABS[::faced];
  ::footed(page, tab);
  GUI::set(page, EMPTY, GUI::Visibility{false});
  GUI::set(page, EMPTY, GUI::Text{tab.empty});
}

void SOUND::VIEWS::TRACE::read() { ::TABS[::faced].read(); }

auto SOUND::VIEWS::TRACE::faced() -> Whole { return ::faced; }

auto SOUND::VIEWS::TRACE::GET::list() -> STRING::Hot {
  return ::TABS[::faced].list;
}
