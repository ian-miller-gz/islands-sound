// SPDX-License-Identifier: AGPL-3.0-or-later
#include "inventory.internal.hpp"
#include "inventory.riff.hpp"

namespace {
using namespace SOUND;

constexpr Whole PCM = 1;
constexpr Whole WIDTH = 2;
constexpr Whole WIDEST = 2;

auto folded(const Vector<Byte> &bytes, Whole at) -> Float {
  return Float(INVENTORY::RIFF::sampled(bytes, at)) / SCALE;
}

void lay(
  const Vector<Byte> &bytes, const INVENTORY::RIFF::Shape &shape,
  Vector<Vector<Float>> &into) {
  const Whole stride = shape.channels * shape.width;
  const Whole total = shape.size / stride;
  into.assign(CHANNELS, {});
  Whole frame = 0;
  Whole remainder = 0;
  while (frame < total) {
    const Float fraction = Float(remainder) / Float(RATE);
    const Whole next = frame + 1 < total ? frame + 1 : frame;
    for (Whole channel = 0; channel < CHANNELS; ++channel) {
      const Whole offset =
        shape.at + (channel < shape.channels ? channel : 0) * shape.width;
      const Float here = ::folded(bytes, offset + frame * stride);
      const Float there = ::folded(bytes, offset + next * stride);
      into[channel].push_back(here + (there - here) * fraction);
    }
    remainder += shape.rate;
    frame += remainder / RATE;
    remainder %= RATE;
  }
}

}  // namespace

auto SOUND::INVENTORY::take(Whole stock, const String &path) -> STRING::Hot {
  Inventory &pool = standing();
  if (stock >= pool.stocks.size()) return "no such stock";
  if (pool.stocks[stock].kind != KIND::AUDIO)
    return "the stock carries no take";
  Vector<Byte> bytes;
  if (IO::read(path, bytes) != 0) return "unreadable";
  INVENTORY::RIFF::Shape shape;
  if (!INVENTORY::RIFF::survey(bytes, shape)) return "not a WAVE";
  if (shape.format != PCM || shape.width != WIDTH) return "not 16-bit PCM";
  if (shape.channels == 0 || shape.channels > WIDEST) return "too many lanes";
  if (shape.size < shape.channels * shape.width) return "no frames in it";
  pool.stocks[stock].take.path = path;
  ::lay(bytes, shape, pool.stocks[stock].take.lanes);
  stir(stock);
  return "";
}

auto SOUND::INVENTORY::take(Whole stock, const Vector<Vector<Float>> &lanes)
  -> STRING::Hot {
  Inventory &pool = standing();
  if (stock >= pool.stocks.size()) return "no such stock";
  if (pool.stocks[stock].kind != KIND::AUDIO)
    return "the stock carries no take";
  if (lanes.empty() || lanes[0].empty()) return "no frames in it";
  pool.stocks[stock].take = {.path = {}, .lanes = lanes};
  stir(stock);
  return "";
}
