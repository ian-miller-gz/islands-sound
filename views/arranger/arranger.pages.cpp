// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../kind.hpp"
#include "../lane/lane.face.hpp"
#include "../views.split.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::BELOW;
using VIEWS::SPLIT::UPPER;

constexpr STRING::Hot SESSION = "session";

Whole paging[std::size(VIEWS::SPLIT::REGIONS)] = {NONE, NONE};
Flag seen[std::size(VIEWS::SPLIT::REGIONS)] = {true, false};

}  // namespace

auto SOUND::VIEWS::ARRANGER::region() -> Whole {
  return VIEWS::document() == SPLIT::second() ? BELOW : UPPER;
}

auto SOUND::VIEWS::ARRANGER::turned() -> Whole { return ::paging[region()]; }

auto SOUND::VIEWS::ARRANGER::paged(Whole region) -> Whole {
  return ::paging[region];
}

auto SOUND::VIEWS::ARRANGER::turned(Whole region, Whole kind) -> Flag {
  if (kind != NONE && !LANE::editable(kind)) return false;
  // One sheet, one of each face: an ask for the face the other region
  // already shows is refused (the second page draws none of the arranger
  // yet), so no region ever takes the other's face from it.
  const Whole other = region == UPPER ? BELOW : UPPER;
  const Flag same = (kind == NONE) == (::paging[other] == NONE);
  if (SPLIT::shared() && ::seen[other] && same) return false;
  ::paging[region] = kind;
  ::seen[region] = true;
  own();
  return true;
}

auto SOUND::VIEWS::ARRANGER::turned(Whole kind) -> Flag {
  return turned(region(), kind);
}

auto SOUND::VIEWS::ARRANGER::seen(Whole region) -> Flag {
  return ::seen[region];
}

auto SOUND::VIEWS::ARRANGER::spoken() -> STRING::Hot {
  return spoken(region());
}

auto SOUND::VIEWS::ARRANGER::spoken(Whole region) -> STRING::Hot {
  const Whole page = ::paging[region];
  return page == NONE ? ::SESSION : KIND::spoken(page);
}
