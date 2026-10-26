// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <iterator>

#include "../../kind.hpp"
#include "../../score.hpp"
#include "../lane/lane.face.hpp"
#include "../views.hpp"
#include "arranger.face.hpp"

namespace SOUND::VIEWS::ARRANGER {

constexpr STRING::Hot NAME = "arranger";
inline constexpr Whole SESSION = 0;

struct Page {
  STRING::Hot word = "";
  STRING::Hot label = "";
  Whole kind = NONE;
};

inline constexpr Page PAGES[] = {
  {"session", "Arrangement"},
  {"notes", "Notes", KIND::NOTES},
  {"control", "Control", KIND::CONTROL},
  {"program", "Program", KIND::PROGRAM}};
inline constexpr Whole COUNT = std::size(PAGES);

auto turned() -> Whole;
auto turned(Whole page) -> Flag;

auto faced() -> const View *;

void label(GUI::Handle page);

void door();

auto paged(Whole kind) -> Whole;

auto offered() -> LANE::Mark;

void keep(FIELDS::Map &face);

void restore(const FIELDS::Map &face);

void shed();

auto below(GUI::Handle page) -> Flag;
void below(SHELL::Session &session);

void focus(const View &face);
auto focused() -> const View *;

}  // namespace SOUND::VIEWS::ARRANGER
