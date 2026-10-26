// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "views.hpp"

namespace SOUND::VIEWS::LIT {

struct Row {
  String id;
  STRING::Hot style = "";
};

void clear();

}  // namespace SOUND::VIEWS::LIT

namespace SOUND::VIEWS {

auto lit() -> Vector<LIT::Row>;

void light(GUI::Handle page, const String &id, STRING::Hot style);

}  // namespace SOUND::VIEWS
