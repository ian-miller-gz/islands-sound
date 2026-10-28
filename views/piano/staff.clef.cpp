// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "piano.paper.hpp"
#include "piano.pips.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace PAPER = SOUND::VIEWS::ROLL::PAPER;
namespace SIGNS = SOUND::VIEWS::ROLL::STAFF::SIGNS;

constexpr STRING::Hot INK = "staffstem", GROUND = "staffpaper";
constexpr STRING::Hot PLATE = "paper";
constexpr Float AIR = ROLL::HALF * ROLL::SPACE;
constexpr Float BARE = 0.0f, NORTH = ROLL::STEP;

Flag stood = false;
Whole born = 0;
Float east = ROLL::RAILED;

auto named(STRING::Hot stem) -> String { return ROLL::board() + "." + stem; }

void build(GUI::Handle page, Flag wanted) {
  const String plate = ::named(::PLATE);
  const String inks[] = {
    SIGNS::clef(0), SIGNS::clef(1), SIGNS::named(0), SIGNS::named(1)};
  if (!wanted) {
    for (const String &id : inks) BOARDS::drop(page, id);
    ROLL::STAFF::lined(page, plate, ::BARE, 0);
    return BOARDS::drop(page, plate);
  }
  SIGNS::inked(page, plate, "panel", PAPER::PLATE, ::GROUND);
  GUI::set(page, plate.c_str(), GUI::Border{::BARE});
  for (const String &id : inks)
    SIGNS::inked(page, id, "label", PAPER::INK, ::INK);
}

void seat(GUI::Handle page) {
  const Float down = GUI::NGA::GET::zoom(page, ROLL::board().c_str()).down;
  const GUI::Extent span = ROLL::STAFF::spanned();
  const Float lines[] = {span.w + ROLL::HALF * span.h, span.w + span.h};
  const Float west = ROLL::RAILED + ::AIR;
  const Float clef = SIGNS::clefed(page, west, lines, down);
  const Float column = west + clef + ::AIR;
  const Float wide =
    column + SIGNS::metred(page, column, lines, down) + ::AIR - ROLL::RAILED;
  const String plate = ::named(::PLATE);
  ::east = ROLL::RAILED + wide;
  GUI::set(page, plate.c_str(), GUI::Position{ROLL::RAILED, ::NORTH});
  GUI::set(
    page, plate.c_str(), GUI::Extent{wide, ROLL::STAFF::deep() - ::NORTH});
  ROLL::STAFF::lined(page, plate, wide, ROLL::STAFF::GET::rules().size());
}

}  // namespace

void SOUND::VIEWS::ROLL::STAFF::clef() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::stood = false;
  const Flag wanted = papered();
  if (wanted != ::stood) ::build(page, wanted);
  ::stood = wanted;
  if (wanted) ::seat(page);
}

auto SOUND::VIEWS::ROLL::margin() -> Float { return ::stood ? ::east : RAILED; }

void SOUND::VIEWS::ROLL::STAFF::SHED::clef() {
  if (VIEWS::reborn(::born) || !::stood) return;
  ::build(document(), false);
  ::stood = false;
}
