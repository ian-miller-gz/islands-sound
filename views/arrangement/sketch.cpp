// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;
using VIEWS::ARRANGEMENT::Pip;

constexpr STRING::Hot GROUND = "ink";
constexpr STRING::Hot INKED = "sketch";

Vector<Whole> stood;
Vector<Flag> bedded;
Whole born = 0;

auto ground(const String &box) -> String { return box + "." + ::GROUND; }

void bare(GUI::Handle page, Whole row, const String &bed) {
  if (::stood[row] != 0) BOARDS::sweep(page, bed.c_str(), 0, ::stood[row]);
  ::stood[row] = 0;
  if (!::bedded[row]) return;
  BOARDS::drop(page, bed);
  ::bedded[row] = false;
}

void inked(GUI::Handle page, Whole row, const Box &box) {
  const String id =
    GUI::SAC::SEAT::named(VIEWS::ARRANGEMENT::board().c_str(), row);
  const String bed = ::ground(id);
  if (!VIEWS::ARRANGEMENT::sketched(box.band)) return ::bare(page, row, bed);
  const Vector<Pip> marks = VIEWS::ARRANGEMENT::pips(box);
  const Float wide = VIEWS::ARRANGEMENT::across(box.frames);
  const Float deep = VIEWS::ARRANGEMENT::inlaid(box.band);
  if (!::bedded[row]) {
    BOARDS::place(page, id.c_str(), "panel", bed, {}, {});
    ::bedded[row] = true;
  }
  for (Whole mark = ::stood[row]; mark < marks.size(); ++mark) {
    const String pip = GUI::SAC::SEAT::named(bed.c_str(), mark);
    BOARDS::place(page, bed.c_str(), "panel", pip, {}, {});
    GUI::set(page, pip.c_str(), GUI::Style{::INKED});
  }
  BOARDS::sweep(page, bed.c_str(), marks.size(), ::stood[row]);
  ::stood[row] = marks.size();
  for (Whole mark = 0; mark < marks.size(); ++mark) {
    const String pip = GUI::SAC::SEAT::named(bed.c_str(), mark);
    GUI::set(
      page, pip.c_str(),
      GUI::Position{marks[mark].at * wide, marks[mark].down * deep});
    GUI::set(
      page, pip.c_str(),
      GUI::Extent{marks[mark].run * wide, marks[mark].deep * deep});
  }
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::sketch(SHELL::Session &session) {
  const Vector<Box> &laid = boxes();
  for (Whole row = 0; row < laid.size(); ++row) {
    const Vector<Pip> marks = pips(laid[row]);
    Float deepest = 0.0f;
    for (const Pip &mark : marks) deepest = std::max(deepest, mark.deep);
    session.print(std::format(
      "sketch {} marks {} deepest {:.2f}", row, marks.size(), deepest));
  }
}

void SOUND::VIEWS::ARRANGEMENT::SHED::sketch() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) {
    ::stood.clear();
    ::bedded.clear();
    return;
  }
  for (Whole row = 0; row < ::stood.size(); ++row)
    ::bare(page, row, ::ground(GUI::SAC::SEAT::named(board().c_str(), row)));
  ::stood.clear();
  ::bedded.clear();
}

void SOUND::VIEWS::ARRANGEMENT::sketch() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) {
    ::stood.clear();
    ::bedded.clear();
  }
  const Vector<Box> &laid = boxes();
  if (::stood.size() > laid.size()) {
    ::stood.resize(laid.size());
    ::bedded.resize(laid.size());
  }
  ::stood.resize(laid.size(), 0);
  ::bedded.resize(laid.size(), false);
  for (Whole row = 0; row < laid.size(); ++row) ::inked(page, row, drawn(row));
}
