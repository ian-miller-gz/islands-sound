// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <iterator>

#include "../../kind.hpp"
#include "../../score.hpp"
#include "../views.hpp"

namespace SOUND::VIEWS::ARRANGER {

constexpr STRING::Hot NAME = "arranger";

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

void door();

void shed();

}  // namespace SOUND::VIEWS::ARRANGER
