// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../arrangement/arrangement.face.hpp"
#include "../lane/lane.face.hpp"
#include "../views.split.regions.hpp"
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

void twin(GUI::Handle page);

}  // namespace

auto SOUND::VIEWS::ARRANGER::facing(Whole region) -> const View & {
  return paged(region) == NONE ? ARRANGEMENT::face() : LANE::face();
}

void SOUND::VIEWS::ARRANGER::shared(GUI::Handle page) {
  if (twinned()) return ::twin(page);
  const View &above = facing(UPPER), &below = facing(BELOW);
  for (const View *unit : ::standers()) {
    VIEWS::show(*unit, true);
    GUI::set(
      page, (String(unit->name) + "." + ::TOOLS).c_str(),
      GUI::Visibility{unit == &facing(SPLIT::focused())});
  }
  ::placed(page, above.name, ::WHOLE, SPLIT::height());
  ::placed(page, below.name, SPLIT::top(), ::WHOLE);
  const Whole held = paged(UPPER) != NONE   ? UPPER
                     : paged(BELOW) != NONE ? BELOW
                                            : NONE;
  const LANE::Mark mark = offered();
  const Whole kind = mark.kind != NONE ? mark.kind : NONE;
  if (held != NONE && kind != NONE) {
    LANE::paged(kind);
    if (kind != paged(held)) turned(held, kind);
  }
  listen(SPLIT::focused() == UPPER);
  ATTENTION::stand(UPPER);
  above.run();
  listen(SPLIT::focused() == BELOW);
  ATTENTION::stand(BELOW);
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

namespace {

void twin(GUI::Handle page) {
  using namespace SOUND::VIEWS;
  const Whole region = side() == 0 ? UPPER : BELOW;
  const View &face = ARRANGER::facing(region);
  for (const View *unit : ::standers()) {
    show(*unit, unit == &face);
    GUI::set(
      page, (String(unit->name) + "." + ::TOOLS).c_str(),
      GUI::Visibility{unit == &face && SPLIT::focused() == region});
  }
  ::placed(page, face.name, ::WHOLE, side() == 0 ? SPLIT::height() : ::WHOLE);
  if (ARRANGER::paged(region) != NONE) LANE::paged(ARRANGER::paged(region));
  listen(SPLIT::focused() == region);
  face.run();
  listen(true);
  ::laid = true;
}

}  // namespace
