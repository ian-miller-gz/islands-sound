// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdint>

#include "inventory.riff.hpp"

namespace {

constexpr Whole TAG = 4;
constexpr Whole WIDE = 4;
constexpr Whole PAIR = 2;
constexpr Whole SPAN = 8;
constexpr Whole LEAD = 12;
constexpr Whole SPEC = 16;
constexpr Whole BITS = 8;

auto le(const Vector<Byte> &bytes, Whole at, Whole width) -> Whole {
  Whole value = 0;
  for (Whole index = 0; index < width; ++index)
    value |= static_cast<Whole>(static_cast<unsigned char>(bytes[at + index]))
             << (BITS * index);
  return value;
}

auto tagged(const Vector<Byte> &bytes, Whole at, STRING::Hot tag) -> Flag {
  for (Whole index = 0; index < TAG; ++index)
    if (bytes[at + index] != tag[index]) return false;
  return true;
}

}  // namespace

auto SOUND::INVENTORY::RIFF::survey(const Vector<Byte> &bytes, Shape &shape)
  -> Flag {
  if (
    bytes.size() < LEAD || !::tagged(bytes, 0, "RIFF") ||
    !::tagged(bytes, SPAN, "WAVE"))
    return false;
  for (Whole at = LEAD; at + SPAN <= bytes.size();) {
    const Whole size = ::le(bytes, at + TAG, WIDE);
    const Whole body = at + SPAN;
    if (
      ::tagged(bytes, at, "fmt ") && size >= SPEC &&
      body + SPEC <= bytes.size()) {
      shape.format = ::le(bytes, body, PAIR);
      shape.channels = ::le(bytes, body + PAIR, PAIR);
      shape.rate = ::le(bytes, body + PAIR + PAIR, WIDE);
      shape.width = ::le(bytes, body + SPEC - PAIR, PAIR) / BITS;
    }
    if (::tagged(bytes, at, "data")) {
      shape.at = body;
      shape.size = std::min(size, static_cast<Whole>(bytes.size() - body));
    }
    at = body + size + (size & 1u);
  }
  return shape.rate != 0 && shape.size != 0;
}

auto SOUND::INVENTORY::RIFF::sampled(const Vector<Byte> &bytes, Whole at)
  -> Integer {
  return static_cast<int16_t>(::le(bytes, at, PAIR));
}
