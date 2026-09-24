// SPDX-License-Identifier: AGPL-3.0-or-later
#include "control.internal.hpp"

void SOUND::CONTROL::turn(const Address &address, Float value) {
  Live &live = standing();
  if (live.taking)
    return live.take.turned.push_back(
      {.address = address, .at = reached(live), .value = value});
  for (Value &touched : values())
    if (same(touched.address, address)) {
      touched.value = value;
      return;
    }
  values().push_back({.address = address, .value = value});
}
