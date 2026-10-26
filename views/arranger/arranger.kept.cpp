// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../lane/lane.face.hpp"
#include "../views.kept.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PAGE = "page";

auto paged() -> String { return String(VIEWS::ARRANGER::NAME) + "." + ::PAGE; }

}  // namespace

void SOUND::VIEWS::ARRANGER::keep(FIELDS::Map &face) {
  face[::paged()] = String(PAGES[turned()].word);
  LANE::keep(face);
}

void SOUND::VIEWS::ARRANGER::restore(const FIELDS::Map &face) {
  String word;
  if (KEPT::text(face, ::paged(), word))
    for (const Page &one : PAGES)
      if (word == one.word) turned(static_cast<Whole>(&one - PAGES));
  LANE::restore(face);
}
