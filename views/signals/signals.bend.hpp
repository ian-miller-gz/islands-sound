// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "signals.runs.hpp"

namespace SOUND::VIEWS::SIGNALS {

void bend();

auto bow() -> GUI::SAC::STROKE::Mode;

auto bowed(Whole row) -> Float;

auto under(Float x, Float y) -> Whole;

auto hovered() -> Flag;

auto closed(const Vector<Point> &rows, Whole row) -> Whole;

void traced(
  GUI::Handle page, const String &id, const Vector<Point> &rows, Whole row);

void segments(SHELL::Session &session);

}  // namespace SOUND::VIEWS::SIGNALS
