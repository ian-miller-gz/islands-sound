// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../inventory.hpp"
#include "piano.heads.hpp"
#include "piano.notation.hpp"
#include "piano.pips.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace HEADS = SOUND::VIEWS::ROLL::HEADS;

namespace VELOCITY {
constexpr Float GHOST = 0.35f, ACCENT = 0.9f;
}  // namespace VELOCITY

constexpr STRING::Hot PARENTHESES = "\U0001D156", STRESS = "\U0001D17B";
constexpr Float MIDDLE = ROLL::HALF * ROLL::SPACE;
constexpr Float FULL = ROLL::SPACE * ROLL::STAFF::SCALE, STILL = 0.0f;
constexpr Float OUTSIDE = MIDDLE + ROLL::SPACE, FLUSH = 0.0f;
constexpr Float WAYS[] = {1.0f, -1.0f};

struct Scale {
  Float wide = 0.0f, tall = 0.0f, aside = 0.0f;
};

auto worn(GUI::Handle page, const String &id, STRING::Hot text, Flag shown)
  -> Flag {
  GUI::set(page, id.c_str(), GUI::Visibility{shown});
  GUI::set(page, id.c_str(), GUI::Text{shown ? text : ""});
  if (!shown) GUI::set(page, id.c_str(), GUI::Extent{});
  return shown;
}

void seat(
  GUI::Handle page, const String &id, const HEADS::Child &child, Scale scale,
  Float lift) {
  GUI::set(
    page, id.c_str(),
    GUI::Position{
      ::MIDDLE - (::MIDDLE - child.at.x) * scale.wide + scale.aside,
      ::MIDDLE - (::MIDDLE - child.at.y - lift) * scale.tall});
  GUI::set(
    page, id.c_str(),
    GUI::Extent{child.size.w * scale.wide, child.size.h * scale.tall});
  GUI::set(page, id.c_str(), GUI::Size{child.letter * scale.wide});
}

auto lift(Whole pitch) -> Float {
  const ROLL::Drum *drum = ROLL::GET::drum(pitch);
  return ::WAYS[drum != nullptr && drum->foot] * ::OUTSIDE;
}

}  // namespace

auto SOUND::VIEWS::ROLL::HEADS::banded(const Pip &pip) -> Whole {
  const Inventory &pool = INVENTORY::held();
  if (pip.stock >= pool.stocks.size()) return EVEN;
  const Score &score = pool.stocks[pip.stock].score;
  if (pip.index >= score.notes.size()) return EVEN;
  const Float velocity = score.notes[pip.index].velocity;
  if (velocity < ::VELOCITY::GHOST) return SOFT;
  return velocity > ::VELOCITY::ACCENT ? LOUD : EVEN;
}

void SOUND::VIEWS::ROLL::HEADS::stressed(
  GUI::Handle page, Whole row, const Worn &now, Float aside) {
  const Flag struck = now.notation == PERCUSSION;
  const Flag plain = String(ROLL::headed(now.pitch)) == ROLL::PLAIN;
  const String ghost = named(row, GHOST), accent = named(row, ACCENT);
  const Flag soft =
    ::worn(page, ghost, ::PARENTHESES, struck && plain && now.loud == SOFT);
  const Flag loud = ::worn(page, accent, ::STRESS, struck && now.loud == LOUD);
  const GUI::NGA::Zoom zoom = GUI::NGA::GET::zoom(page, board().c_str());
  if (zoom.value <= ::STILL || zoom.down <= ::STILL) return;
  const Scale scale = {
    now.side / ::FULL / zoom.value, now.side / ::FULL / zoom.down, aside};
  if (soft) ::seat(page, ghost, GHOST, scale, ::FLUSH);
  if (loud) ::seat(page, accent, ACCENT, scale, ::lift(now.pitch));
}
