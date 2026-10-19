// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../graph.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::SHUT;

struct Pair {
  STRING::Hot above = "", below = "";
};

constexpr Pair PAIRS[] = {
  {"graph", ""}, {"params", ""}, {"arranger", ""}};

Whole below = NONE;

auto row(const String &name) -> Whole {
  const Vector<VIEWS::View> &units = VIEWS::roster();
  for (Whole at = 0; at < units.size(); ++at)
    if (name == units[at].name) return at;
  return NONE;
}

auto viewed(Whole at) -> Flag {
  return at < VIEWS::roster().size() && !VIEWS::roster()[at].panel;
}

auto fit(Whole at) -> Flag {
  return ::viewed(at) && (at != VIEWS::chosen() || VIEWS::SPLIT::hosted(at));
}

auto paired() -> Whole {
  for (const ::Pair &pair : ::PAIRS)
    if (String(pair.above) == VIEWS::standing() && ::fit(::row(pair.below)))
      return ::row(pair.below);
  for (Whole at = 0; at < VIEWS::roster().size(); ++at)
    if (::viewed(at) && at != VIEWS::chosen()) return at;
  return NONE;
}

auto held() -> Whole { return ::fit(::below) ? ::below : ::paired(); }

auto icon(const VIEWS::View &unit) -> String {
  return String(VIEWS::SPLIT::BAR) + "." + unit.name;
}

}  // namespace

auto SOUND::VIEWS::SPLIT::open() -> Flag {
  const GUI::Handle page = sheet();
  const Float bar = GUI::GET::extent(page, BAR).h;
  const Float least = GUI::GET::extent(page, BAND).h;
  return height() > ::SHUT && height() - bar - least >= least;
}

auto SOUND::VIEWS::SPLIT::lower() -> Whole { return open() ? ::held() : NONE; }

auto SOUND::VIEWS::SPLIT::choose(const String &name) -> Flag {
  const Whole at = ::row(name);
  if (!::viewed(at)) return false;
  const Whole now = lower();
  if (at == chosen() && !hosted(at) && now != NONE)
    return VIEWS::choose(String(roster()[now].name));
  ::below = at;
  return true;
}

void SOUND::VIEWS::SPLIT::traded(Whole row) {
  if (row != chosen() && row == lower()) ::below = chosen();
}

void SOUND::VIEWS::SPLIT::state(SHELL::Session &session) {
  const Whole at = lower();
  if (at == NONE) return;
  session.print(
    std::format("split {} {} {}", height(), LOWER, roster()[at].name));
}

void SOUND::VIEWS::SPLIT::chose() {
  if (open()) ::below = ::held();
  const Vector<View> &units = roster();
  for (const View &unit : units)
    if (!unit.panel && GUI::GET::clicked(sheet(), ::icon(unit).c_str()))
      choose(unit.name);
  for (Whole at = 0; at < units.size(); ++at)
    if (!units[at].panel)
      GUI::set(
        sheet(), ::icon(units[at]).c_str(),
        GUI::Style{at == lower() ? MARK : PLAIN});
}
