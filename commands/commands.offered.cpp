// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "commands.offer.hpp"

namespace {
using namespace SOUND;

auto held() -> Vector<SHELL::Command> & {
  static Vector<SHELL::Command> rows;
  return rows;
}

}  // namespace

auto SOUND::COMMANDS::offer(const Vector<SHELL::Command> &rows) -> Flag {
  ::held().insert(::held().end(), rows.begin(), rows.end());
  return true;
}

auto SOUND::COMMANDS::offered() -> Vector<SHELL::Command> {
  Vector<SHELL::Command> rows = ::held();
  std::ranges::stable_sort(
    rows, [](const SHELL::Command &one, const SHELL::Command &two) {
      return String(one.name) < String(two.name);
    });
  return rows;
}
