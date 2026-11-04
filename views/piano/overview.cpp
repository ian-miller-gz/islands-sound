// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../transport.hpp"
#include "piano.internal.hpp"
#include "piano.rows.get.hpp"

namespace {
using namespace SOUND;

constexpr Float ORIGIN = 0.0f;

auto folded(Float lane, Float deep) -> Float {
  return std::max(lane, ::ORIGIN) / deep;
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
  const ROWS::Ladder &ladder = ROWS::GET::ladder();
  const Float deep = ladder.deep();
  for (const Pip &note : plates()) {
    read.reach = std::max(read.reach, note.at + note.length);
    const Float lane = ladder.lane(note.pitch, note.at);
    read.dashes.push_back(
      {across(note.at), across(note.length), ::folded(lane, deep)});
  }
  return read;
}
