// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iterator>
#include <string_view>

#include "../fade.hpp"
#include "views.curves.hpp"

namespace {
using namespace SOUND;

constexpr Float STRAIGHT = 0.0f;
constexpr Float BOWED = 0.25f;

struct Row {
  STRING::Hot key;
  VIEWS::Curve curve;
};

constexpr Row CURVES[] = {
  {"line", {GUI::Shape::LINE, ::STRAIGHT}},
  {"slow", {GUI::Shape::RISE, ::STRAIGHT}},
  {"fast", {GUI::Shape::FALL, ::STRAIGHT}},
  {"even", {GUI::Shape::EASE, ::STRAIGHT}},
  {"power", {GUI::Shape::FALL, ::BOWED}},
};

constexpr auto covered() -> Flag {
  for (const FADE::Shape &shape : FADE::SHAPES) {
    Whole rows = 0;
    for (const Row &row : CURVES)
      rows += Whole(std::string_view(shape.word) == row.key);
    if (rows != 1) return false;
  }
  return std::size(CURVES) == std::size(FADE::SHAPES);
}
static_assert(covered());

constexpr STRING::Hot NAMES[] = {"line", "", "fall", "ease", "hold"};
static_assert(std::size(NAMES) == GUI::Shape::HOLD + 1);

}  // namespace

auto SOUND::VIEWS::curve(Whole shape) -> Curve {
  if (shape >= std::size(FADE::SHAPES)) return {};
  const std::string_view word = FADE::SHAPES[shape].word;
  for (const ::Row &row : ::CURVES)
    if (word == row.key) return row.curve;
  return {};
}

auto SOUND::VIEWS::spoken(GUI::Shape::Run run) -> STRING::Hot {
  return run < std::size(::NAMES) ? ::NAMES[run] : ::NAMES[GUI::Shape::LINE];
}
