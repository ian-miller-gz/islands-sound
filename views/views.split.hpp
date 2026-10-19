// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.hpp"

namespace SOUND::VIEWS::SPLIT {

constexpr STRING::Hot BAR = "split";
constexpr STRING::Hot BAND = "split.views";
constexpr STRING::Hot LOWER = "lower";
constexpr Float SHUT = 0.0f;

void frame();

auto height() -> Float;

auto open() -> Flag;

auto lower() -> Whole;

auto host(STRING::Hot name) -> Flag;
auto hosted(Whole row) -> Flag;

auto shared() -> Flag;

auto top() -> Float;

auto choose(const String &name) -> Flag;

void traded(Whole row);

void keep(FIELDS::Map &face);
void restore(const FIELDS::Map &face);

void state(SHELL::Session &session);

void chose();

}  // namespace SOUND::VIEWS::SPLIT
