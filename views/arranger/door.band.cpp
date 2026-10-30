// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../views.split.hpp"
#include "door.hpp"

namespace {
using namespace SOUND;
namespace ARRANGER = SOUND::VIEWS::ARRANGER;
namespace SPLIT = SOUND::VIEWS::SPLIT;
using SPLIT::BELOW;

constexpr STRING::Hot HINT = ".hint";

Whole born = 0;

auto stands(Whole region) -> Flag {
  if (region != BELOW) return String(VIEWS::standing()) == ARRANGER::NAME;
  const Whole at = SPLIT::lower();
  return at != NONE && String(VIEWS::roster()[at].name) == ARRANGER::NAME;
}

void asked(GUI::Handle page, const ARRANGER::Half &half) {
  const String &id = ARRANGER::paired(BELOW).*half.id;
  if (!GUI::GET::clicked(page, id.c_str()) && !SPLIT::struck(id)) return;
  if (!::stands(BELOW)) SPLIT::choose(ARRANGER::NAME);
  ARRANGER::ask(BELOW, half.face());
  SPLIT::focus(BELOW);
}

void worded(GUI::Handle page) {
  if (!VIEWS::reborn(::born)) return;
  const String strip = ARRANGER::paired(SPLIT::UPPER).door + ::HINT;
  const String band = ARRANGER::paired(BELOW).door + ::HINT;
  GUI::set(page, band.c_str(), GUI::Text{GUI::GET::text(page, strip.c_str())});
}

}  // namespace

auto SOUND::VIEWS::ARRANGER::faced(Whole region) -> const View * {
  if (!::stands(region)) return nullptr;
  return paged(region) == NONE ? &ARRANGEMENT::face() : &LANE::face();
}

void SOUND::VIEWS::ARRANGER::ask(Whole region, const View &face) {
  turned(region, &face == &LANE::face() ? offered().kind : NONE);
}

void SOUND::VIEWS::ARRANGER::BAND::door(GUI::Handle page) {
  for (const Half &half : HALVES) ::asked(page, half);
  ::worded(page);
}
