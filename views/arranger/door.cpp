// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../views.internal.hpp"
#include "../views.lit.hpp"
#include "../views.split.hpp"
#include "door.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::ARRANGER;
namespace LANE = SOUND::VIEWS::LANE;
namespace SPLIT = SOUND::VIEWS::SPLIT;

constexpr STRING::Hot RESTING = "pair";
constexpr STRING::Hot HELD = "pairlit";
constexpr STRING::Hot PLATE = ".set";

auto create(const String &icon, Whole region) -> Pair {
  return {icon, icon + SPLIT::DOOR, icon + ::PLATE, region};
}

auto strip() -> const Pair & { return paired(SPLIT::UPPER); }

auto editing(const LANE::Mark &mark) -> Flag {
  if (String(VIEWS::standing()) != NAME) return false;
  return faced() == &LANE::face() && turned() == mark.kind;
}

void opened(GUI::Handle page) {
  const String &id = ::strip().door;
  if (!GUI::GET::clicked(page, id.c_str()) && !SPLIT::struck(id)) return;
  const LANE::Mark mark = offered();
  if (::editing(mark)) {
    turned(SPLIT::UPPER, NONE);
    VIEWS::choose(NAME);
  } else if (mark.lane != NONE) {
    LANE::mark(mark.track, mark.lane);
    turned(SPLIT::UPPER, mark.kind);
    VIEWS::choose(NAME);
  }
}

void arrived() {
  if (!VIEWS::ARRANGEMENT::GET::opened()) return;
  const LANE::Mark mark = LANE::marked();
  if (mark.lane != NONE) edit(mark.track, mark.lane);
}

void returned(GUI::Handle page) {
  const String &id = ::strip().icon;
  if (!GUI::GET::clicked(page, id.c_str()) && !SPLIT::struck(id)) return;
  turned(SPLIT::UPPER, NONE);
  VIEWS::choose(NAME);
}

}  // namespace

auto SOUND::VIEWS::ARRANGER::paired(Whole region) -> const Pair & {
  static const Pair pairs[] = {
    ::create(VIEWS::icon(NAME), SPLIT::UPPER),
    ::create(String(SPLIT::BAR) + "." + NAME, SPLIT::BELOW)};
  return pairs[region];
}

void SOUND::VIEWS::ARRANGER::light(GUI::Handle page, const Pair &pair) {
  const STRING::Hot mark = SPLIT::mark(pair.region);
  const View *face = faced(pair.region);
  for (const Half &half : HALVES)
    VIEWS::light(page, pair.*half.id, face == &half.face() ? mark : PLAIN);
  const String over = String(GUI::GET::hover(page));
  const Flag rested = over == pair.icon || over == pair.door;
  GUI::set(page, pair.plate.c_str(), GUI::Style{rested ? ::HELD : ::RESTING});
}

void SOUND::VIEWS::ARRANGER::door() {
  const GUI::Handle page = document();
  BAND::door(page);
  ::returned(page);
  ::opened(page);
  ::arrived();
  light(page, paired(SPLIT::BELOW));
  light(page, ::strip());
}
