// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <chrono>

#include "../../inventory.hpp"
#include "signals.bend.hpp"

namespace {
using namespace SOUND;
using Clock = std::chrono::steady_clock;

constexpr STRING::Hot MODE = "select";
constexpr STRING::Hot BENT = "bend";
constexpr Float BOW = 30.0f;
constexpr Float LIMIT = 4.0f;
constexpr Float STRAIGHT = 0.0f;
constexpr std::chrono::milliseconds DOUBLE{400};

struct Hold {
  Whole row = NONE, stock = NONE, index = NONE;
  Float from = 0.0f, was = 0.0f, bend = 0.0f;
};
Hold holding;

struct Tap {
  Whole row = NONE;
  Clock::time_point at;
};
Tap tapped;

void opened(const GUI::SAC::STROKE::Gesture &gesture) {
  if (!gesture.alt) return;
  const Whole row = VIEWS::SIGNALS::under(gesture.from.at.x, gesture.from.at.y);
  if (row == NONE) return;
  const VIEWS::SIGNALS::Point &point = VIEWS::SIGNALS::shown()[row];
  ::holding = {row,        point.stock, point.index, gesture.from.at.y,
               point.bend, point.bend};
}

void wrote(const ::Hold &hold, Float bend) {
  if (bend == hold.was || hold.stock >= INVENTORY::held().stocks.size()) return;
  HISTORY::take(hold.stock, INVENTORY::held().stocks[hold.stock]);
  INVENTORY::bend(hold.stock, hold.index, bend);
  HISTORY::wrote(::BENT, hold.stock, INVENTORY::held().stocks[hold.stock]);
}

void closed(const GUI::SAC::STROKE::Gesture &gesture) {
  const ::Hold hold = ::holding;
  ::holding = {};
  if (gesture.dragged) return ::wrote(hold, hold.bend);
  const Flag paired =
    ::tapped.row == hold.row && Clock::now() - ::tapped.at <= ::DOUBLE;
  ::tapped = paired ? ::Tap{} : ::Tap{hold.row, Clock::now()};
  if (paired) ::wrote(hold, ::STRAIGHT);
}

}  // namespace

void SOUND::VIEWS::SIGNALS::bend() {
  if (GUI::SAC::STROKE::GET::mode(document()) != ::MODE)
    return (void)(::holding = {});
  const GUI::SAC::STROKE::Gesture gesture = VIEWS::GET::gesture();
  if (gesture.opened) ::opened(gesture);
  if (::holding.row == NONE) return;
  ::holding.bend = std::clamp(
    ::holding.was + (gesture.to.at.y - ::holding.from) / ::BOW, -::LIMIT,
    ::LIMIT);
  if (gesture.closed) ::closed(gesture);
}

auto SOUND::VIEWS::SIGNALS::bow() -> GUI::SAC::STROKE::Mode {
  if (::holding.row == NONE && !hovered()) return {};
  return {.board = board(), .name = ::MODE};
}

auto SOUND::VIEWS::SIGNALS::bowed(Whole row) -> Float {
  if (row == ::holding.row) return ::holding.bend;
  return row < shown().size() ? shown()[row].bend : 0.0f;
}
