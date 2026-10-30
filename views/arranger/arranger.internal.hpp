// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <span>

#include "../../kind.hpp"
#include "../../score.hpp"
#include "../lane/lane.face.hpp"
#include "../views.hpp"
#include "arranger.face.hpp"

namespace SOUND::VIEWS::ARRANGER {

constexpr STRING::Hot NAME = "arranger";

auto region() -> Whole;

auto turned() -> Whole;

auto paged(Whole region) -> Whole;

auto turned(Whole region, Whole kind) -> Flag;

auto seen() -> Flag;
auto turned(Whole kind) -> Flag;

auto spoken() -> STRING::Hot;

auto spoken(Whole region) -> STRING::Hot;

auto faced() -> const View *;

void label(GUI::Handle page);

void door();

auto offered() -> LANE::Mark;

void keep(FIELDS::Map &face);

void restore(const FIELDS::Map &face);

void shed();

auto faced(Whole region) -> const View *;
void ask(Whole region, const View &face);

auto own() -> Flag;
}  // namespace SOUND::VIEWS::ARRANGER

namespace SOUND::VIEWS::ARRANGER::TOOLS {

void seat(GUI::Handle page);

void restore(std::span<const GUI::Position> kept);

}  // namespace SOUND::VIEWS::ARRANGER::TOOLS

namespace SOUND::VIEWS::ARRANGER::TOOLS::GET {

auto places() -> std::span<const GUI::Position>;

}  // namespace SOUND::VIEWS::ARRANGER::TOOLS::GET
