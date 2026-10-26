// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../lane/lane.face.hpp"
#include "../views.kept.hpp"
#include "../views.split.hpp"
#include "door.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PAGE = "page";
constexpr STRING::Hot UNDER = "under";
constexpr STRING::Hot X = ".x";
constexpr STRING::Hot Y = ".y";

auto key(STRING::Hot leaf) -> String {
  return String(VIEWS::ARRANGER::NAME) + "." + leaf;
}

void under(const FIELDS::Map &face) {
  String name;
  if (!VIEWS::KEPT::text(face, ::key(::UNDER), name)) return;
  for (const auto &half : VIEWS::ARRANGER::HALVES)
    if (name == half.face().name) VIEWS::ARRANGER::lower(half.face());
}

void turned(const FIELDS::Map &face, Whole kind) {
  namespace LANE = VIEWS::LANE;
  if (kind == NONE || LANE::marked(face)) {
    VIEWS::ARRANGER::turned(kind);
    return;
  }
  const LANE::Mark mark = LANE::edited(kind);
  if (mark.lane != NONE) LANE::mark(mark.track, mark.lane);
  VIEWS::ARRANGER::turned(NONE);
}

void placed(FIELDS::Map &face) {
  const auto places = VIEWS::ARRANGER::TOOLS::GET::places();
  for (Whole at = 0; at < places.size(); ++at) {
    const String stem = ::key(VIEWS::SPLIT::REGIONS[at]);
    face[stem + ::X] = places[at].x;
    face[stem + ::Y] = places[at].y;
  }
}

void placed(const FIELDS::Map &face) {
  GUI::Position places[std::size(VIEWS::SPLIT::REGIONS)];
  Flag whole = true;
  for (Whole at = 0; at < std::size(places); ++at) {
    const String stem = ::key(VIEWS::SPLIT::REGIONS[at]);
    whole = VIEWS::KEPT::number(face, stem + ::X, places[at].x) && whole;
    whole = VIEWS::KEPT::number(face, stem + ::Y, places[at].y) && whole;
  }
  if (whole) VIEWS::ARRANGER::TOOLS::restore(places);
}

}  // namespace

void SOUND::VIEWS::ARRANGER::keep(FIELDS::Map &face) {
  face[::key(::PAGE)] = String(spoken());
  face[::key(::UNDER)] = String(lowered().name);
  ::placed(face);
  LANE::keep(face);
}

void SOUND::VIEWS::ARRANGER::restore(const FIELDS::Map &face) {
  LANE::restore(face);
  ::under(face);
  ::placed(face);
  String word;
  if (KEPT::text(face, ::key(::PAGE), word)) ::turned(face, KIND::meant(word));
}
