// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdint>
#include <fstream>

#include "master.hpp"

namespace {
using namespace SOUND;

using Pulse = int16_t;

constexpr Float PEAK = 32767.0f;
constexpr Whole HEAD = 36;
constexpr Whole SPEC = 16;
constexpr Whole PCM = 1;
constexpr Whole BYTE = 8;

auto folded(Float value) -> Pulse {
  return static_cast<Pulse>(std::clamp(value, -1.0f, 1.0f) * ::PEAK);
}

void little(std::ofstream &out, Whole value, Whole width) {
  for (Whole index = 0; index < width; ++index)
    out.put(static_cast<char>((value >> (::BYTE * index)) & 0xffu));
}

auto longest(const Vector<Vector<Float>> &lanes) -> Whole {
  Whole frames = 0;
  for (const Vector<Float> &lane : lanes)
    frames = std::max<Whole>(frames, lane.size());
  return frames;
}

}  // namespace

auto SOUND::MASTER::WAVE::write(
  const String &path, Whole rate, const Vector<Vector<Float>> &lanes) -> Flag {
  std::ofstream out(path, std::ios::binary);
  if (!out || lanes.empty()) return false;
  const Whole frames = ::longest(lanes);
  const Whole align = lanes.size() * sizeof(Pulse);
  const Whole bytes = frames * align;
  out.write("RIFF", 4), ::little(out, ::HEAD + bytes, 4), out.write("WAVE", 4);
  out.write("fmt ", 4), ::little(out, ::SPEC, 4), ::little(out, ::PCM, 2);
  ::little(out, lanes.size(), 2), ::little(out, rate, 4);
  ::little(out, rate * align, 4), ::little(out, align, 2);
  ::little(out, ::BYTE * sizeof(Pulse), 2);
  out.write("data", 4), ::little(out, bytes, 4);
  for (Whole frame = 0; frame < frames; ++frame)
    for (const Vector<Float> &lane : lanes)
      ::little(
        out,
        static_cast<uint16_t>(
          ::folded(frame < lane.size() ? lane[frame] : 0.0f)),
        2);
  return out.good();
}
