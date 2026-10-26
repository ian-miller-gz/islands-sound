// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.split.hpp"

namespace SOUND::VIEWS::SPLIT::GROUND {

constexpr STRING::Hot UPPER = "split.upper";
constexpr STRING::Hot LOWER = "split.lower";

}  // namespace SOUND::VIEWS::SPLIT::GROUND

namespace SOUND::VIEWS::SPLIT {

constexpr STRING::Hot MAIN = "main";
constexpr STRING::Hot RIM = "split.focus";
constexpr STRING::Hot TOGGLE = "views.split";
constexpr STRING::Hot HANDLE = "split.handle";
constexpr Float SHARE = 0.4f;
constexpr char DOT = '.';

struct Seat {
  Float foot = SHUT, depth = SHUT, bar = SHUT, least = SHUT, room = SHUT;
};

auto measured(GUI::Handle page) -> Seat;

void rooted(GUI::Handle page, const Seat &seat, Float tall);

void grounded(GUI::Handle page, const Seat &seat, Float tall);

auto heads() -> Vector<String>;

auto landed(GUI::Handle page) -> String;

auto placed(const String &id) -> Whole;

auto spotted(GUI::Handle page) -> Whole;

void pressed(GUI::Handle page);

auto last() -> Float;

void toggle();

void restore(Float tall, Float kept);

auto read(const Vector<Byte> &bytes) -> Vector<String>;

void dress(GUI::Handle page, const Vector<String> &roots);

void board(FIELDS::Map &face);
void boarded(const FIELDS::Map &face);

}  // namespace SOUND::VIEWS::SPLIT

namespace SOUND::VIEWS::SPLIT::GET {

auto handle() -> STRING::Hot;

}  // namespace SOUND::VIEWS::SPLIT::GET

namespace SOUND::VIEWS::SPLIT::HOLD {

void take();

auto keyed() -> Flag;

void shown(Flag on);

}  // namespace SOUND::VIEWS::SPLIT::HOLD
