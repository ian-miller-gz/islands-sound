// SPDX-License-Identifier: AGPL-3.0-or-later
#include "inventory.internal.hpp"

namespace {
using namespace SOUND;
using INVENTORY::RIFF::Shape;

constexpr Whole HEAD = 65536;
constexpr Whole PCM = 1;
constexpr Whole WIDTH = 2;
constexpr Whole WIDEST = 2;
constexpr STRING::Hot FOREIGN = "not WAVE";

struct Refusal {
  Flag (*fails)(const Shape &shape);
  STRING::Hot word;
};

constexpr Refusal REFUSALS[] = {
  {[](const Shape &shape) -> Flag {
     return shape.format != PCM || shape.width != WIDTH;
   },
   "not 16-bit PCM"},
  {[](const Shape &shape) -> Flag {
     return shape.channels == 0 || shape.channels > WIDEST;
   },
   "too many lanes"},
  {[](const Shape &shape) -> Flag {
     return shape.size < shape.channels * shape.width;
   },
   "no frames"}};

}  // namespace

auto SOUND::INVENTORY::refused(const Vector<Byte> &bytes, RIFF::Shape &shape)
  -> STRING::Hot {
  if (!RIFF::survey(bytes, shape)) return ::FOREIGN;
  for (const ::Refusal &refusal : ::REFUSALS)
    if (refusal.fails(shape)) return refusal.word;
  return "";
}

auto SOUND::INVENTORY::heard(const String &path) -> Heard {
  IO::STREAMS::Input file(path, std::ios::binary);
  if (!file) return {.refusal = UNREADABLE};
  Vector<Byte> bytes(::HEAD);
  file.read(bytes.data(), ::HEAD);
  bytes.resize(file.gcount());
  RIFF::Shape shape;
  const STRING::Hot why = refused(bytes, shape);
  if (why[0] != '\0') return {.refusal = why};
  return {
    .channels = shape.channels,
    .rate = shape.rate,
    .frames = shape.declared / (shape.channels * shape.width)};
}
