// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../graph.hpp"
#include "views.internal.hpp"
#include "views.split.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::SHUT;

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

auto doubles(Whole at) -> Flag {
  return VIEWS::SPLIT::mirrored(at) && VIEWS::SPLIT::second() != GUI::NONE;
}

auto fit(Whole at) -> Flag {
  const Flag twin = VIEWS::SPLIT::hosted(at) || ::doubles(at);
  return ::viewed(at) && (at != VIEWS::chosen() || twin);
}

auto paired() -> Whole {
  for (Whole at = 0; at < VIEWS::roster().size(); ++at)
    if (::viewed(at) && VIEWS::SPLIT::hosted(at)) return at;
  for (Whole at = 0; at < VIEWS::roster().size(); ++at)
    if (::viewed(at) && at != VIEWS::chosen()) return at;
  return NONE;
}

auto held() -> Whole { return ::fit(::below) ? ::below : ::paired(); }

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
  ::below = at;
  focus(BELOW);
  return true;
}

void SOUND::VIEWS::SPLIT::traded(Whole) { focus(UPPER); }
