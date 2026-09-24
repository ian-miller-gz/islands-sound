// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "arrangement.internal.hpp"

namespace SOUND::VIEWS::ARRANGEMENT {

struct Stand {
  Whole track = 0, lane = 0;
  String id;
  String name, standing;
  STRING::Hot kind = "";
  String dress;
  Flag laden = false;
  Flag hears = false;
  Flag shelved = false;
};

void cells(const Vector<Stand> &stands);

void doors();
void doors(const Vector<Stand> &stands);
auto shut(const String &track, const String &lane) -> Flag;

auto shut(Whole band) -> Flag;

}  // namespace SOUND::VIEWS::ARRANGEMENT
