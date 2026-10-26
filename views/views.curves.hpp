// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/gui.hpp>

namespace SOUND::VIEWS {

struct Curve {
  GUI::Shape::Run run = GUI::Shape::LINE;
  Float bend = 0.0f;
};

constexpr Float TOLERANCE = 0.02f;

auto curve(Whole shape) -> Curve;

auto spoken(GUI::Shape::Run run) -> STRING::Hot;

struct Fall {
  GUI::Position at;
  Float run = 0.0f, deep = 0.0f;
  Whole shape = 0;
  Float weight = 0.0f;
};

void stroke(GUI::Handle page, const String &id, const Fall &fall);

}  // namespace SOUND::VIEWS
