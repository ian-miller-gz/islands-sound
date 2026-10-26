// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "graph.internal.hpp"

namespace SOUND::VIEWS::WIRED::BROWSE {

constexpr STRING::Hot CROSSING = "crossing";
constexpr Float SIZED = 15.0f;
constexpr Float READ = 14.0f;
constexpr Float AIR = 12.0f;
constexpr Float RIM = 16.0f;
constexpr Float SHARE = 0.6f;

struct Fit {
  Float across = 0.0f;
  Float column = 0.0f;
  Float wide = 0.0f;
};

auto cut(const Fit &whole, Float run, Float named, Float room) -> Fit;
void crop(
  GUI::Handle page, const String &cell, const String &name,
  const String &crossing, const Fit &fit);
void tell(SHELL::Session &session, GUI::Handle page);

}  // namespace SOUND::VIEWS::WIRED::BROWSE
