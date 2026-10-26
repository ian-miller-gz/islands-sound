// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;

constexpr STRING::Hot WESTED = "endwest";
constexpr STRING::Hot EASTED = "endeast";
constexpr STRING::Hot GRIPPED = "grip";

Vector<Flag> stood;
Whole born = 0;

void handed(GUI::Handle page, Whole row) {
  const String id =
    GUI::SAC::SEAT::named(VIEWS::ARRANGEMENT::board().c_str(), row);
  const String west = id + "." + ::WESTED;
  const String east = id + "." + ::EASTED;
  if (!::stood[row]) {
    for (const String &end : {west, east}) {
      BOARDS::place(page, id.c_str(), "node", end, {}, {});
      GUI::set(page, end.c_str(), GUI::Style{::GRIPPED});
      GUI::set(page, end.c_str(), GUI::Depth{VIEWS::ARRANGEMENT::RAISED});
    }
    ::stood[row] = true;
  }
  const Box box = VIEWS::ARRANGEMENT::drawn(row);
  const Flag shown = VIEWS::ARRANGEMENT::gripped(box.frames);
  const Float wide = VIEWS::ARRANGEMENT::GRIP / VIEWS::ARRANGEMENT::scaled();
  const Float deep = VIEWS::ARRANGEMENT::inlaid(box.band);
  GUI::set(page, west.c_str(), GUI::Visibility{shown});
  GUI::set(page, east.c_str(), GUI::Visibility{shown});
  if (!shown) return;
  GUI::set(page, west.c_str(), GUI::Position{0.0f, 0.0f});
  GUI::set(page, west.c_str(), GUI::Extent{wide, deep});
  GUI::set(
    page, east.c_str(),
    GUI::Position{VIEWS::ARRANGEMENT::across(box.frames) - wide, 0.0f});
  GUI::set(page, east.c_str(), GUI::Extent{wide, deep});
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::ends() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) ::stood.clear();
  const Vector<Box> &laid = boxes();
  if (::stood.size() > laid.size()) ::stood.resize(laid.size());
  ::stood.resize(laid.size(), false);
  for (Whole row = 0; row < laid.size(); ++row) ::handed(page, row);
}
