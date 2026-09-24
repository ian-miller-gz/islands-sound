// SPDX-License-Identifier: AGPL-3.0-or-later
#include "session.doors.hpp"
#include "session.store.internal.hpp"

namespace {
constexpr STRING::Hot TABLE = "faces";
}  // namespace

void SOUND::SESSION::keep(const String &name, const FIELDS::Map &face) {
  if (name.empty()) return;
  STORE::put(store(), ::TABLE, name.c_str(), face);
}

auto SOUND::SESSION::kept(const String &name, FIELDS::Map &face) -> Flag {
  return !name.empty() && STORE::get(store(), ::TABLE, name.c_str(), face);
}
