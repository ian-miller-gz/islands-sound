// SPDX-License-Identifier: AGPL-3.0-or-later
#include "session.doors.hpp"
#include "session.store.internal.hpp"

namespace {
constexpr STRING::Hot TABLE = "preferences";
}  // namespace

void SOUND::SESSION::settle(const String &key, const FIELDS::Map &row) {
  const STORE::Handle opened = store();
  if (opened.id != 0) STORE::put(opened, ::TABLE, key.c_str(), row);
}

auto SOUND::SESSION::settled(const String &key, FIELDS::Map &row) -> Flag {
  const STORE::Handle opened = store();
  return opened.id != 0 && STORE::get(opened, ::TABLE, key.c_str(), row);
}
