// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <utility>

#include "../views.split.regions.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float ORIGIN = 0.0f;

GUI::Position places[std::size(VIEWS::SPLIT::REGIONS)];
GUI::Position written;
Flag known = false, stale = true;
Whole born = 0;

auto top(Whole region) -> Float {
  return region == VIEWS::SPLIT::BELOW ? VIEWS::SPLIT::top() : ::ORIGIN;
}

auto seated(Whole region) -> GUI::Position {
  return {::places[region].x, ::top(region) + ::places[region].y};
}

auto roomed(GUI::Handle page, Whole region, GUI::Position wanted)
  -> GUI::Position {
  const VIEWS::View *unit = VIEWS::found(VIEWS::ARRANGER::NAME);
  if (unit == nullptr) return wanted;
  const Float spare =
    VIEWS::SPLIT::depth(region) - VIEWS::SPLIT::hung(page, *unit);
  if (spare < ::ORIGIN) return wanted;
  const Float top = ::top(region);
  return {wanted.x, std::clamp(wanted.y, top, top + spare)};
}

void heard(GUI::Handle page, Whole region) {
  if (!GUI::GET::moved(page, VIEWS::ARRANGER::TOOLS::HANDLE)) return;
  const GUI::Position at =
    GUI::GET::position(page, VIEWS::ARRANGER::TOOLS::HANDLE);
  ::places[region] = {at.x, at.y - ::top(region)};
  ::written = at;
}

}  // namespace

void SOUND::VIEWS::ARRANGER::TOOLS::seat(GUI::Handle page) {
  if (VIEWS::reborn(::born)) ::stale = true;
  if (!std::exchange(::known, true))
    std::ranges::fill(
      ::places, GUI::GET::position(page, VIEWS::ARRANGER::TOOLS::HANDLE));
  const Whole at = SPLIT::shared() ? SPLIT::focused() : SPLIT::UPPER;
  ::heard(page, at);
  const GUI::Position wanted = ::roomed(page, at, ::seated(at));
  ::places[at].y = wanted.y - ::top(at);
  if (!::stale && wanted == ::written) return;
  GUI::set(page, VIEWS::ARRANGER::TOOLS::HANDLE, wanted);
  ::written = wanted;
  ::stale = false;
}

auto SOUND::VIEWS::ARRANGER::TOOLS::GET::places()
  -> std::span<const GUI::Position> {
  return ::places;
}

void SOUND::VIEWS::ARRANGER::TOOLS::restore(
  std::span<const GUI::Position> kept) {
  std::ranges::copy(kept.first(std::size(::places)), ::places);
  ::known = true;
  ::stale = true;
}
