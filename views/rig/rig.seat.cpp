// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../views.letters.hpp"
#include "rig.hpp"

namespace {
using namespace SOUND;
namespace RIG = SOUND::VIEWS::RIG;

constexpr Float SHARE = 0.5f;

auto id(const String &cell, STRING::Hot part) -> String {
  return cell + "." + part;
}

auto at(GUI::Handle page, STRING::Hot part) -> GUI::Position {
  return GUI::GET::position(page, ::id(RIG::PATTERN, part).c_str());
}

auto part(GUI::Handle page, STRING::Hot part) -> GUI::Extent {
  return GUI::GET::extent(page, ::id(RIG::PATTERN, part).c_str());
}

}  // namespace

auto SOUND::VIEWS::RIG::column(
  GUI::Handle page, STRING::Hot list, const Vector<String> &names) -> Float {
  Whole widest = 0;
  for (const String &name : names)
    widest = std::max(widest, UNICODE::columns(name));
  const Float run =
    Float(widest) * LETTERS::ADVANCE * LETTER + ::at(page, NAMED).x + AIR;
  return std::min(run, ::SHARE * GUI::GET::measured(page, list).w);
}

void SOUND::VIEWS::RIG::seat(
  GUI::Handle page, const String &cell, Float column, Float room, Float gap) {
  const GUI::Position door = ::at(page, STEPPED);
  const GUI::Position dial = ::at(page, DIALLED);
  const GUI::Extent run = ::part(page, DIALLED);
  const Float west = column + dial.x - door.x;
  const Float wide = std::clamp(room - west, 0.0f, run.w);
  const GUI::Extent name = {
    column - ::at(page, NAMED).x - AIR, ::part(page, NAMED).h};
  GUI::set(page, ::id(cell, NAMED).c_str(), name);
  GUI::set(page, ::id(cell, STEPPED).c_str(), GUI::Position{column, door.y});
  GUI::set(page, ::id(cell, DIALLED).c_str(), GUI::Position{west, dial.y});
  GUI::set(page, ::id(cell, DIALLED).c_str(), GUI::Extent{wide, run.h});
  const Float east = west + wide + AIR;
  const GUI::Position restore = {east, ::at(page, RESTORED).y};
  GUI::set(page, ::id(cell, RESTORED).c_str(), restore);
  const GUI::Position mark = {east + gap, ::at(page, BOUND).y};
  GUI::set(page, ::id(cell, BOUND).c_str(), mark);
}
