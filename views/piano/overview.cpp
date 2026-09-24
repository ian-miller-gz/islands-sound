// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../transport.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

auto folded(Whole pitch) -> Float {
  const Whole held = std::min(pitch, VIEWS::ROLL::HIGHEST);
  return Float(VIEWS::ROLL::HIGHEST - held) / Float(VIEWS::ROLL::HIGHEST + 1);
}

}  // namespace

namespace {

auto placed(Whole frames) -> Float {
  return SOUND::VIEWS::ROLL::across(SOUND::VIEWS::ROLL::opened(frames));
}

}  // namespace

auto SOUND::VIEWS::ROLL::overview() -> VIEWS::Overview {
  Overview read;
  read.placed = ::placed;
  read.reach = PULSES * TRANSPORT::metred().numerator;
  for (const Pip &note : plates()) {
    read.reach = std::max(read.reach, note.at + note.length);
    read.dashes.push_back(
      {across(note.at), across(note.length), ::folded(note.pitch)});
  }
  return read;
}
