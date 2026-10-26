// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "wave.internal.hpp"

namespace SOUND::VIEWS::WAVE {

constexpr STRING::Hot OVER = "waveover";
constexpr Float FULL = 1.0f;
constexpr Float DEPTH = 32768.0f;
constexpr Float CEILING = FULL - FULL / DEPTH;

auto clipped(Float least, Float most) -> Flag;
void dressed(GUI::Handle page, const String &id, Whole at);
void dressed(
  GUI::Handle page, const String &id, Whole at, const PEAKS::Peak &peak);
void overs(SHELL::Session &session);

}  // namespace SOUND::VIEWS::WAVE
