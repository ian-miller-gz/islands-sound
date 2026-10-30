// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../arrangement/arrangement.face.hpp"
#include "../lane/lane.face.hpp"
#include "../views.split.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::BELOW;
using VIEWS::SPLIT::UPPER;

struct Head {
  STRING::Hot word = "";
  const VIEWS::View &(*face)() = nullptr;
};

constexpr Head HEADS[] = {
  {"arrangement", VIEWS::ARRANGEMENT::face},
  {"lane", VIEWS::LANE::face},
  {"piano", VIEWS::LANE::face},
  {"signals", VIEWS::LANE::face},
  {"wave", VIEWS::LANE::face}};

}  // namespace

auto SOUND::VIEWS::ARRANGER::own() -> Flag {
  const Flag lowered = SPLIT::shared() ? paged(BELOW) != NONE : false;
  const Flag raised = SPLIT::shared() ? paged(UPPER) == NONE : true;
  for (const ::Head &head : ::HEADS) {
    const Flag lane = head.face == VIEWS::LANE::face;
    SPLIT::owned(head.word, NAME, lane ? (lowered ? BELOW : UPPER) : (raised ? UPPER : BELOW));
  }
  return true;
}
