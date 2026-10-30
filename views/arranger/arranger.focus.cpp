// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../arrangement/arrangement.face.hpp"
#include "../lane/lane.face.hpp"
#include "../views.split.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;
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
  for (const ::Head &head : ::HEADS) SPLIT::owned(head.word, NAME, UPPER);
  return true;
}
