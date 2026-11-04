// SPDX-License-Identifier: AGPL-3.0-or-later
#include "control.internal.hpp"
#include "control.map.hpp"

namespace {
using namespace SOUND;

Vector<Value> standing;
Vector<Value> rests;

auto stood(const Address &address) -> const Value * {
  for (const Value &value : ::standing)
    if (CONTROL::same(value.address, address)) return &value;
  return nullptr;
}

}  // namespace

auto SOUND::CONTROL::values() -> Vector<Value> & { return ::standing; }

auto SOUND::CONTROL::rests() -> Vector<Value> & { return ::rests; }

auto SOUND::CONTROL::held() -> const Vector<Value> & { return ::standing; }

void SOUND::CONTROL::adopt(const Vector<Value> &values) {
  ::standing = values;
  MAP::adopt({});
}

auto SOUND::CONTROL::same(const Address &one, const Address &two) -> Flag {
  return one.parameter == two.parameter && one.node == two.node;
}

auto SOUND::CONTROL::resting(const Address &address, Float memory) -> Float {
  const Value *stood = ::stood(address);
  if (stood != nullptr) return stood->value;
  for (const Value &rest : ::rests)
    if (same(rest.address, address)) return rest.value;
  ::rests.push_back({.address = address, .value = memory});
  return memory;
}

auto SOUND::CONTROL::sounding(const Address &address, const Offer &offer)
  -> Sounded {
  const Value *stood = ::stood(address);
  if (offer.arced) return {.from = ARC, .value = offer.arc};
  if (stood != nullptr && stood->overridden)
    return {.from = VALUE, .value = stood->value};
  if (offer.governed) return {.from = CURVE, .value = offer.curve};
  if (stood != nullptr) return {.from = VALUE, .value = stood->value};
  return {.from = MEMORY, .value = offer.memory};
}
