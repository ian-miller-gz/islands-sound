// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../rig/rig.hpp"
#include "wave.placed.hpp"

namespace SOUND::VIEWS::WAVE {

constexpr STRING::Hot ROWS = "wave.rows";

namespace STRETCH {

auto source() -> RIG::Source;

void commit();

}  // namespace STRETCH

namespace SHIFT {

auto pitch() -> RIG::Source;

auto cents() -> RIG::Source;

void commit();

}  // namespace SHIFT

namespace ONSETS {

auto flag() -> RIG::Source;

auto sense() -> RIG::Source;

}  // namespace ONSETS

namespace NOTES {

auto flag() -> RIG::Source;

}  // namespace NOTES

namespace TUNE {

auto source() -> RIG::Source;

void commit();

}  // namespace TUNE

namespace FADES {

auto sources() -> Vector<RIG::Source>;

void doors(GUI::Handle page, Whole first);

}  // namespace FADES

void fit(GUI::Handle page, Whole rows);

void rows();
void rows(SHELL::Session &session);

auto worded(const Clip &clip) -> String;

}  // namespace SOUND::VIEWS::WAVE
