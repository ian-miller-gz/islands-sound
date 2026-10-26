// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>
#include <island/gui/stroke.hpp>
#include <island/input.hpp>

namespace SOUND::VIEWS {

void polled();
auto struck() -> INPUT::KEYS::Event;
void listen(Flag on);
auto listening() -> Flag;
void draw(const GUI::SAC::STROKE::Mode &mode);
auto escaped() -> Flag;
auto journaled(const INPUT::KEYS::Event &key) -> Flag;

auto folded(Whole code) -> Whole;

struct Chord {
  INPUT::KEYS::Action action = INPUT::KEYS::TEXT;
  Whole code = 0;
  Flag control = false, shift = false;
};

auto chorded(const Chord &chord) -> Flag;

}  // namespace SOUND::VIEWS

namespace SOUND::VIEWS::GET {

auto gesture() -> GUI::SAC::STROKE::Gesture;

}  // namespace SOUND::VIEWS::GET
