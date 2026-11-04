// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.hpp"

namespace SOUND::VIEWS::SPLIT {

constexpr Whole UPPER = 0, BELOW = 1;
constexpr STRING::Hot REGIONS[] = {"upper", "lower"};

auto height() -> Float;

auto open() -> Flag;

auto lower() -> Whole;

auto second() -> GUI::Handle;
auto doubled() -> Flag;
auto turned() -> Flag;
auto home() -> GUI::Handle;

struct Turn {
  explicit Turn(Flag on);
  ~Turn();
  Turn(const Turn &) = delete;
  auto operator=(const Turn &) -> Turn & = delete;
  Flag on = false;
};

auto shared() -> Flag;

auto top() -> Float;

auto focused() -> Whole;

auto aimed() -> const View *;

}  // namespace SOUND::VIEWS::SPLIT
