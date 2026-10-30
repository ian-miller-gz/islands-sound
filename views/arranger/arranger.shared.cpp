// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../arrangement/arrangement.face.hpp"
#include "../lane/lane.face.hpp"
#include "../views.split.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::BELOW;
using VIEWS::SPLIT::UPPER;

constexpr STRING::Hot TOOLS = "tools";
constexpr Float WHOLE = 0.0f;

Flag laid = false;

auto standers() -> Vector<const VIEWS::View *> {
  return {&VIEWS::ARRANGEMENT::face(), &VIEWS::LANE::face()};
}

void placed(GUI::Handle page, STRING::Hot root, Float down, Float margin) {
  const GUI::Position at = GUI::GET::position(page, root);
  const GUI::Extent size = GUI::GET::extent(page, root);
  if (at.y != down) GUI::set(page, root, GUI::Position{at.x, down});
  if (size.h != margin) GUI::set(page, root, GUI::Extent{size.w, margin});
}

}  // namespace

auto SOUND::VIEWS::ARRANGER::facing(Whole region) -> const View & {
  return paged(region) == NONE ? ARRANGEMENT::face() : LANE::face();
}

/// @brief The arranger in both regions: its two faces stand on the one
/// sheet, the upper region's page whole above the bar and the lower's from
/// the bar's foot, each listening when its region holds the focus.
void SOUND::VIEWS::ARRANGER::shared(GUI::Handle page) {
  const View &above = facing(UPPER), &below = facing(BELOW);
  const Flag same = &above == &below;
  for (const View *unit : ::standers()) {
    VIEWS::show(*unit, !same || unit == &above);
    GUI::set(
      page, (String(unit->name) + "." + ::TOOLS).c_str(),
      GUI::Visibility{unit == &facing(SPLIT::focused())});
  }
  ::placed(page, above.name, ::WHOLE, SPLIT::height());
  if (same) {
    SPLIT::echo(
      GUI::GET::origin(page, above.name), GUI::GET::measured(page, above.name));
  } else {
    ::placed(page, below.name, SPLIT::top(), ::WHOLE);
  }
  const Whole held = paged(UPPER) != NONE   ? UPPER
                     : paged(BELOW) != NONE ? BELOW
                                            : NONE;
  const LANE::Mark mark = offered();
  const Whole kind = mark.kind != NONE ? mark.kind : NONE;
  if (held != NONE && kind != NONE) {
    LANE::paged(kind);
    if (kind != paged(held)) turned(held, kind);
  }
  if (same) {
    above.run();
    ::laid = true;
    return;
  }
  listen(SPLIT::focused() == UPPER);
  above.run();
  listen(SPLIT::focused() == BELOW);
  below.run();
  listen(true);
  ::laid = true;
}

void SOUND::VIEWS::ARRANGER::joined(GUI::Handle page) {
  if (!::laid) return;
  for (const View *unit : ::standers())
    ::placed(page, unit->name, ::WHOLE, ::WHOLE);
  ::laid = false;
}
