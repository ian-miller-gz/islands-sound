// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "history.hpp"

namespace SOUND::HISTORY {

void stir(const Edit &edit);
void stir();

void folded(Edit &tail, const Edit &fresh);

auto standing() -> Whole;

auto arced() -> Edit *;

void differed(
  Whole stock, const Stock &stood, const Stock &stands, Vector<Noted> &notes,
  Vector<Plotted> &plots);

void released();

void walked(const Edit &edit, Flag back);

}  // namespace SOUND::HISTORY
