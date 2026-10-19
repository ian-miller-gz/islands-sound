// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "piano.internal.hpp"

namespace SOUND::VIEWS::ROLL::EASEL {

struct Cell {
  Integer at = 0, pitch = 0;
};

constexpr STRING::Hot PAINTED = "paint";

auto celled(const GUI::SAC::STROKE::Cell &cell) -> Cell;

auto held(const Score &score, const Cell &cell, Whole at, Whole span) -> Whole;

void said(GUI::Handle page, STRING::Hot text);

void open(const Cell &cell);

}  // namespace SOUND::VIEWS::ROLL::EASEL
