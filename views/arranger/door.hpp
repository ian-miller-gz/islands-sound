// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../arrangement/arrangement.face.hpp"
#include "arranger.internal.hpp"

namespace SOUND::VIEWS::ARRANGER {

struct Pair {
  String icon, door, plate;
  Whole region = NONE;
};

struct Half {
  String Pair::*id = nullptr;
  const View &(*face)() = nullptr;
};

constexpr Half HALVES[] = {
  {&Pair::icon, ARRANGEMENT::face}, {&Pair::door, LANE::face}};

auto paired(Whole region) -> const Pair &;
void light(GUI::Handle page, const Pair &pair);

}  // namespace SOUND::VIEWS::ARRANGER

namespace SOUND::VIEWS::ARRANGER::BAND {

void door(GUI::Handle page);

}  // namespace SOUND::VIEWS::ARRANGER::BAND
