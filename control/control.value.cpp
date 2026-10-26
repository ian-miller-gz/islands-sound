// SPDX-License-Identifier: AGPL-3.0-or-later
#include "control.internal.hpp"

namespace {
using namespace SOUND;

Vector<Value> standing;

}  // namespace

auto SOUND::CONTROL::values() -> Vector<Value> & { return ::standing; }

auto SOUND::CONTROL::held() -> const Vector<Value> & { return ::standing; }

void SOUND::CONTROL::adopt(const Vector<Value> &values) { ::standing = values; }

auto SOUND::CONTROL::same(const Address &one, const Address &two) -> Flag {
  return one.parameter == two.parameter && one.node == two.node;
}

auto SOUND::CONTROL::sounding(const Address &address, const Offer &offer)
  -> Sounded {
  if (offer.arced) return {.from = ARC, .value = offer.arc};
  if (offer.governed) return {.from = CURVE, .value = offer.curve};
  for (const Value &value : ::standing)
    if (same(value.address, address))
      return {.from = VALUE, .value = value.value};
  return {.from = MEMORY, .value = offer.memory};
}
