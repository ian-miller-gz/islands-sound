// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../lane/lane.face.hpp"
#include "../views.kept.hpp"
#include "../views.split.hpp"
#include "door.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PAGE = "page";
constexpr STRING::Hot LOWER = "lower.page";
constexpr STRING::Hot X = ".x";
constexpr STRING::Hot Y = ".y";

auto key(STRING::Hot leaf) -> String {
  return String(VIEWS::ARRANGER::NAME) + "." + leaf;
}

void turned(const FIELDS::Map &face, Whole region, Whole kind) {
  namespace LANE = VIEWS::LANE;
  if (kind == NONE || LANE::marked(face)) {
    VIEWS::ARRANGER::turned(region, kind);
    return;
  }
  const LANE::Mark mark = LANE::edited(kind);
  if (mark.lane != NONE) LANE::mark(mark.track, mark.lane);
  VIEWS::ARRANGER::turned(region, NONE);
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
  face[::key(::PAGE)] = String(spoken(SPLIT::UPPER));
  face[::key(::LOWER)] = String(spoken(SPLIT::BELOW));
  ::placed(face);
  LANE::keep(face);
}

void SOUND::VIEWS::ARRANGER::restore(const FIELDS::Map &face) {
  LANE::restore(face);
  ::placed(face);
  String word;
  if (KEPT::text(face, ::key(::PAGE), word))
    ::turned(face, SPLIT::UPPER, KIND::meant(word));
  if (KEPT::text(face, ::key(::LOWER), word))
    ::turned(face, SPLIT::BELOW, KIND::meant(word));
}
