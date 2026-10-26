// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.internal.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;

auto mirrors() -> Vector<String> & {
  static Vector<String> names;
  return names;
}

}  // namespace

auto SOUND::VIEWS::SPLIT::mirror(STRING::Hot name) -> Flag {
  if (name == nullptr || name[0] == '\0') return false;
  ::mirrors().emplace_back(name);
  return true;
}

auto SOUND::VIEWS::SPLIT::mirrored(Whole row) -> Flag {
  if (row >= roster().size()) return false;
  for (const String &name : ::mirrors())
    if (name == roster()[row].name) return true;
  return false;
}
