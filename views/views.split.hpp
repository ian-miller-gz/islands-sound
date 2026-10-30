// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/graphics/backend/passes.hpp>

#include "views.hpp"

namespace SOUND::VIEWS::SPLIT {

constexpr STRING::Hot BAR = "split";
constexpr STRING::Hot BAND = "split.views";
constexpr STRING::Hot LOWER = "lower";
constexpr Float SHUT = 0.0f;
constexpr Whole UPPER = 0, BELOW = 1;
constexpr STRING::Hot REGIONS[] = {"upper", "lower"};
constexpr STRING::Hot DIM = "dimmed";
constexpr STRING::Hot DOOR = ".door";

void frame();

void keyed();

void seat(Float tall);

auto held() -> Flag;

auto struck(const String &id) -> Flag;

auto height() -> Float;

auto open() -> Flag;

auto lower() -> Whole;

auto host(STRING::Hot name) -> Flag;
auto hosted(Whole row) -> Flag;

auto mirror(STRING::Hot name) -> Flag;
auto mirrored(Whole row) -> Flag;

auto doubled() -> Flag;
void echo(GUI::Position at, GUI::Extent size);
void draw(GFX::Pass &pass);

auto shared() -> Flag;

auto top() -> Float;

auto choose(const String &name) -> Flag;

void traded(Whole row);

void keep(FIELDS::Map &face);
void restore(const FIELDS::Map &face);

void state(SHELL::Session &session);

void chose();

auto mark(Whole region) -> STRING::Hot;

auto focused() -> Whole;
void focus(Whole region);

auto aimed() -> const View *;

void owned(STRING::Hot head, STRING::Hot view, Whole region);

}  // namespace SOUND::VIEWS::SPLIT
