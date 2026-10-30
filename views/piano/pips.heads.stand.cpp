// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "piano.pips.hpp"
#include "piano.staff.hpp"
#include "piano.weights.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace HEADS = SOUND::VIEWS::ROLL::HEADS;
namespace WEIGHTS = SOUND::VIEWS::ROLL::WEIGHTS;

struct Glyph {
  Float size = 0.0f, wide = 0.0f, x = 0.0f, y = 0.0f;
};
constexpr auto inked(const ROLL::Ink &ink) -> Glyph {
  return {
    ROLL::EM, (ink.west + ink.wide) * ROLL::EM,
    (ink.west + ROLL::HALF * ink.wide) * ROLL::EM,
    (ROLL::BASELINE - ink.rise) * ROLL::EM};
}
constexpr Glyph HEADED = ::inked(ROLL::INK);
constexpr Glyph MARKED = {24.0f, 12.4f, 6.2f, 13.1f};
constexpr Glyph STRESSED = ::inked({0.05f, 0.416f, -0.146f});
constexpr Glyph BRACKETED = ::inked({0.05f, 0.513f, 0.1425f});
constexpr Glyph SIGNED = {
  ROLL::EM, 0.28f * ROLL::EM, (0.45f + 0.14f) * ROLL::EM,
  (ROLL::BASELINE - 0.135f) * ROLL::EM};

constexpr Float MIDDLE = ROLL::HALF * ROLL::SPACE, SIDES = 2.0f;
constexpr Float INKED = ROLL::INK.wide * ROLL::EM;
constexpr GUI::Extent RULE = {
  ::INKED + ::SIDES * WEIGHTS::REACH * ROLL::SPACE,
  WEIGHTS::LEDGER *ROLL::SPACE};

constexpr auto lettered(STRING::Hot name, const Glyph &glyph) -> HEADS::Child {
  return {
    name,
    "label",
    {::MIDDLE - glyph.x, ::MIDDLE - glyph.y},
    {glyph.wide, glyph.size},
    glyph.size};
}

}  // namespace

const SOUND::VIEWS::ROLL::HEADS::Child SOUND::VIEWS::ROLL::HEADS::HEAD =
  ::lettered("head", ::HEADED);
const SOUND::VIEWS::ROLL::HEADS::Child SOUND::VIEWS::ROLL::HEADS::MARK =
  ::lettered("mark", ::MARKED);
const SOUND::VIEWS::ROLL::HEADS::Child SOUND::VIEWS::ROLL::HEADS::ACCENT =
  ::lettered("accent", ::STRESSED);
const SOUND::VIEWS::ROLL::HEADS::Child SOUND::VIEWS::ROLL::HEADS::GHOST =
  ::lettered("ghost", ::BRACKETED);
const SOUND::VIEWS::ROLL::HEADS::Child SOUND::VIEWS::ROLL::HEADS::SIGN =
  ::lettered("sign", ::SIGNED);
const SOUND::VIEWS::ROLL::HEADS::Child SOUND::VIEWS::ROLL::HEADS::LEDGER = {
  "ledger",
  "panel",
  {::MIDDLE - ROLL::HALF * ::RULE.w, ::MIDDLE - ROLL::HALF * ::RULE.h},
  ::RULE};

namespace {

const HEADS::Child *const CHILDREN[] = {
  &HEADS::HEAD, &HEADS::MARK, &HEADS::LEDGER, &HEADS::ACCENT, &HEADS::GHOST,
  &HEADS::SIGN};

}  // namespace

auto SOUND::VIEWS::ROLL::HEADS::named(Whole row, const Child &child) -> String {
  return GUI::SAC::SEAT::named(board().c_str(), row) + "." + child.name;
}

void SOUND::VIEWS::ROLL::HEADS::stand(GUI::Handle page, Whole row) {
  const String plate = GUI::SAC::SEAT::named(board().c_str(), row);
  for (const Child *child : ::CHILDREN) {
    const String id = named(row, *child);
    BOARDS::place(page, plate.c_str(), child->kind, id, child->at, {});
    GUI::set(page, id.c_str(), GUI::Size{child->letter});
  }
}

void SOUND::VIEWS::ROLL::HEADS::drop(GUI::Handle page, Whole row) {
  for (const Child *child : ::CHILDREN) BOARDS::drop(page, named(row, *child));
}

void SOUND::VIEWS::ROLL::HEADS::ruled(
  GUI::Handle page, Whole row, Float wide, Float aside) {
  const Float reach = WEIGHTS::drawn(WEIGHTS::Across{WEIGHTS::REACH});
  const GUI::Extent size = {
    ::INKED * wide + ::SIDES * reach,
    WEIGHTS::drawn(WEIGHTS::Down{WEIGHTS::LEDGER})};
  const String id = named(row, LEDGER);
  GUI::set(
    page, id.c_str(),
    GUI::Position{::MIDDLE - HALF * size.w + aside, ::MIDDLE - HALF * size.h});
  GUI::set(page, id.c_str(), size);
}
