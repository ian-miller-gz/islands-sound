// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;

constexpr STRING::Hot NOTCHED = "notch";
constexpr STRING::Hot PANEL = "panel";

Vector<Whole> stood;
Whole born = 0;

void bare(GUI::Handle page, Whole row, const String &id) {
  BOARDS::sweep(page, id.c_str(), 0, ::stood[row]);
  ::stood[row] = 0;
}

void seated(GUI::Handle page, Whole row, const String &id, Whole wanted) {
  for (Whole mark = ::stood[row]; mark < wanted; ++mark) {
    const String notch = GUI::SAC::SEAT::named(id.c_str(), mark);
    BOARDS::place(page, id.c_str(), ::PANEL, notch, {}, {});
    GUI::set(page, notch.c_str(), GUI::Style{::NOTCHED});
  }
  BOARDS::sweep(page, id.c_str(), wanted, ::stood[row]);
  ::stood[row] = wanted;
}

void placed(
  GUI::Handle page, const String &id, const Box &box, const Vector<Whole> &at) {
  for (Whole mark = 0; mark < at.size(); ++mark) {
    const String notch = GUI::SAC::SEAT::named(id.c_str(), mark);
    GUI::set(
      page, notch.c_str(),
      GUI::Position{VIEWS::ARRANGEMENT::across(at[mark]), 0.0f});
    GUI::set(
      page, notch.c_str(),
      GUI::Extent{
        VIEWS::ARRANGEMENT::HAIR / VIEWS::ARRANGEMENT::scaled(),
        VIEWS::ARRANGEMENT::inlaid(box.band)});
  }
}

void marked(GUI::Handle page, Whole row, const Box &box) {
  const String id =
    GUI::SAC::SEAT::named(VIEWS::ARRANGEMENT::board().c_str(), row);
  if (!VIEWS::ARRANGEMENT::sketched(box.band)) return ::bare(page, row, id);
  const Vector<Whole> at = VIEWS::ARRANGEMENT::marks(box);
  ::seated(page, row, id, at.size());
  ::placed(page, id, box, at);
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::SHED::notches() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) {
    ::stood.clear();
    return;
  }
  for (Whole row = 0; row < ::stood.size(); ++row)
    BOARDS::sweep(
      page, GUI::SAC::SEAT::named(board().c_str(), row).c_str(), 0,
      ::stood[row]);
  ::stood.clear();
}

void SOUND::VIEWS::ARRANGEMENT::notches() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) ::stood.clear();
  const Vector<Box> &laid = boxes();
  if (::stood.size() > laid.size()) ::stood.resize(laid.size());
  ::stood.resize(laid.size(), 0);
  for (Whole row = 0; row < laid.size(); ++row) ::marked(page, row, laid[row]);
}
