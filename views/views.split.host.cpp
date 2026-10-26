// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../graph.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;

auto hosts() -> Vector<String> & {
  static Vector<String> names;
  return names;
}

}  // namespace

auto SOUND::VIEWS::SPLIT::host(STRING::Hot name) -> Flag {
  if (name == nullptr || name[0] == '\0') return false;
  ::hosts().emplace_back(name);
  return true;
}

auto SOUND::VIEWS::SPLIT::hosted(Whole row) -> Flag {
  if (row >= roster().size()) return false;
  for (const String &name : ::hosts())
    if (name == roster()[row].name) return true;
  return false;
}

auto SOUND::VIEWS::SPLIT::shared() -> Flag {
  const Whole at = lower();
  return at != NONE && at == chosen();
}
