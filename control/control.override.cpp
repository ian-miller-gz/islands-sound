// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "control.internal.hpp"
#include "control.override.hpp"

namespace {
using namespace SOUND;

Vector<String> moved;

auto stood(const Address &address) -> Value * {
  for (Value &value : CONTROL::values())
    if (CONTROL::same(value.address, address)) return &value;
  return nullptr;
}

void mark(Value &value, Flag on) {
  if (value.overridden == on) return;
  value.overridden = on;
  if (std::ranges::find(::moved, value.address.node) == ::moved.end())
    ::moved.push_back(value.address.node);
}

}  // namespace

void SOUND::CONTROL::overrule(const Address &address) {
  Value *value = ::stood(address);
  if (value != nullptr) ::mark(*value, true);
}

auto SOUND::CONTROL::overridden(const Address &address) -> Flag {
  const Value *value = ::stood(address);
  return value != nullptr && value->overridden;
}

auto SOUND::CONTROL::overrides() -> Vector<Address> {
  Vector<Address> addresses;
  for (const Value &value : values())
    if (value.overridden) addresses.push_back(value.address);
  return addresses;
}

void SOUND::CONTROL::restore(const Address &address) {
  Value *value = ::stood(address);
  if (value != nullptr) ::mark(*value, false);
}

void SOUND::CONTROL::restore() {
  for (Value &value : values()) ::mark(value, false);
}

auto SOUND::CONTROL::shifted() -> Vector<String> {
  Vector<String> nodes;
  nodes.swap(::moved);
  return nodes;
}
