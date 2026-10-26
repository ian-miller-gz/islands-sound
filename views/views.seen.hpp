// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <island/gui/nga.hpp>

#include "views.hpp"

namespace SOUND::VIEWS {

struct Seen {
  STRING::Hot name = "";
  GUI::NGA::Pan pan;
  GUI::NGA::Zoom zoom;
};

namespace GET {

auto seen(const View &unit) -> Seen;

auto seen() -> Seen;

}  // namespace GET

}  // namespace SOUND::VIEWS
