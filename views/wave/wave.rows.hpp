// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../rig/rig.hpp"

namespace SOUND::VIEWS::WAVE {

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

void fit(GUI::Handle page, Whole rows);

}  // namespace SOUND::VIEWS::WAVE
