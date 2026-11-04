// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <iterator>

#include "../control.hpp"
#include "../timeline.hpp"
#include "views.attention.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;
namespace ATTENTION = SOUND::VIEWS::ATTENTION;
using VIEWS::SPLIT::BELOW;
using VIEWS::SPLIT::UPPER;

constexpr STRING::Hot LIST = "tracks";
constexpr Whole SHEET = 0;

ATTENTION::Slot slots[ATTENTION::SLOTS];
Whole holds[std::size(VIEWS::SPLIT::REGIONS)] = {};
Whole shown = 0;
Whole standing = NONE;

auto region(Whole row) -> Whole {
  if (row == VIEWS::chosen()) return UPPER;
  return row == VIEWS::SPLIT::lower() ? BELOW : NONE;
}

auto drawn() -> Whole {
  if (!VIEWS::SPLIT::open()) return UPPER;
  if (VIEWS::side() != ::SHEET) return BELOW;
  return ::standing == NONE ? VIEWS::SPLIT::focused() : ::standing;
}

auto listed() -> Whole {
  const Whole track = VIEWS::cursor(::LIST);
  return track < TIMELINE::held().tracks.size() ? track : NONE;
}

}  // namespace

auto SOUND::VIEWS::ATTENTION::held(Whole region) -> Whole {
  return region < std::size(::holds) ? ::holds[region] : ::holds[UPPER];
}

auto SOUND::VIEWS::ATTENTION::hold(Whole region, Whole slot) -> Flag {
  if (region >= std::size(::holds) || slot >= SLOTS) return false;
  ::holds[region] = slot;
  return true;
}

auto SOUND::VIEWS::ATTENTION::get(Whole slot) -> Slot & {
  return ::slots[std::min(slot, SLOTS - 1)];
}

auto SOUND::VIEWS::ATTENTION::current() -> Slot & {
  return ::slots[held(SPLIT::focused())];
}

auto SOUND::VIEWS::ATTENTION::viewed() -> Slot & {
  return ::slots[held(::drawn())];
}

auto SOUND::VIEWS::ATTENTION::attended(Slot &slot) -> Whole {
  if (&slot == &::slots[::shown]) slot.track = ::listed();
  return slot.track;
}

auto SOUND::VIEWS::ATTENTION::attended() -> Whole { return attended(viewed()); }

auto SOUND::VIEWS::ATTENTION::aimed(Slot &slot) -> String {
  if (&slot == &::slots[::shown]) slot.node = CONTROL::aimed();
  return slot.node;
}

auto SOUND::VIEWS::ATTENTION::shown() -> Whole { return ::shown; }

void SOUND::VIEWS::ATTENTION::show(Whole slot) {
  ::shown = std::min(slot, SLOTS - 1);
}

void SOUND::VIEWS::ATTENTION::run(Whole row) {
  ::standing = ::region(row);
  registry()[row].run();
  ::standing = NONE;
}

void SOUND::VIEWS::ATTENTION::stand(Whole region) { ::standing = region; }
