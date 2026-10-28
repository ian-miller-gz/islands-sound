// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include "piano.internal.hpp"
#include "piano.notation.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr STRING::Hot GROUNDS[] = {"board", "staffpaper"};

Whole worn = NONE, born = 0;

}  // namespace

void SOUND::VIEWS::ROLL::STAFF::drawn() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::worn = NONE;
  const Whole staffed = ROLL::GET::staffed() ? 1 : 0;
  if (std::exchange(::worn, staffed) != staffed)
    GUI::set(page, board().c_str(), GUI::Style{::GROUNDS[staffed]});
  ruled(staffed ? GET::rules() : Vector<Integer>{});
}

void SOUND::VIEWS::ROLL::STAFF::SHED::drawn() { ruled({}); }
