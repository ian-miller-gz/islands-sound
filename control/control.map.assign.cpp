// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "control.internal.hpp"

void SOUND::CONTROL::MAP::assign(const Address &address, Whole number) {
  Vector<Binding> &bindings = learned();
  const Binding made = {
    .address = address, .number = number, .channel = chosen().channel};
  const auto found = std::ranges::find_if(bindings, [&](const Binding &held) {
    return CONTROL::same(held.address, address);
  });
  if (found == bindings.end())
    bindings.push_back(made);
  else
    *found = made;
  if (CONTROL::same(listening(), address)) cancel();
}
