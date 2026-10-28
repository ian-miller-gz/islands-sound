// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "piano.heads.hpp"
#include "piano.notation.hpp"
#include "piano.pips.hpp"
#include "piano.staff.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace HEADS = SOUND::VIEWS::ROLL::HEADS;

constexpr Integer PAIRED = 2;

constexpr STRING::Hot INKS[] = {"staffhead", "staffchosen", "staffloose"};

auto inked(const HEADS::Worn &now) -> String {
  if (now.ink != HEADS::RESTING) return ::INKS[now.ink];
  return ROLL::tinted(::INKS[now.ink], now.tier);
}

// A ledger: an even place off every staff — past the rules' ends, or in
// the grand staff's gap, where the rules either side stand more than a
// line apart.
auto ledgered(Integer place) -> Flag {
  const Vector<Integer> rules = ROLL::STAFF::GET::rules();
  if (place == ROLL::STAFF::NOWHERE || place % ::PAIRED != 0) return false;
  if (rules.empty()) return false;
  if (place < rules.front() || place > rules.back()) return true;
  const auto above = std::ranges::lower_bound(rules, place);
  if (above != rules.end() && *above == place) return false;
  return above != rules.begin() && *above - *(above - 1) > ::PAIRED;
}

auto headed(const HEADS::Worn &now) -> STRING::Hot {
  if (!now.pitched) return ROLL::headed(now.pitch);
  return now.hollow ? ROLL::HOLLOW : ROLL::PLAIN;
}

void show(GUI::Handle page, Whole row, const HEADS::Child &child, Flag shown) {
  const String id = HEADS::named(row, child);
  GUI::set(page, id.c_str(), GUI::Visibility{shown});
  GUI::set(page, id.c_str(), shown ? child.size : GUI::Extent{});
}

void paint(GUI::Handle page, Whole row, const HEADS::Worn &now, Float aside) {
  const String ink = ::inked(now);
  for (const HEADS::Child *child :
       {&HEADS::HEAD, &HEADS::MARK, &HEADS::LEDGER, &HEADS::ACCENT,
        &HEADS::GHOST, &HEADS::SIGN})
    GUI::set(page, HEADS::named(row, *child).c_str(), GUI::Style{ink.c_str()});
  HEADS::sized(page, row, now.side, aside);
}

}  // namespace

void SOUND::VIEWS::ROLL::HEADS::dress(
  GUI::Handle page, Whole row, const Worn &now) {
  const Flag struck = now.notation == ROLL::PERCUSSION || now.pitched;
  const STRING::Hot sign = now.pitched ? "" : ROLL::marked(now.pitch);
  const Integer place =
    now.place == ROLL::STAFF::NOWHERE ? ROLL::STAFF::top() : now.place;
  const String mark = named(row, MARK);
  const Float aside = shift(now);
  GUI::set(page, named(row, HEAD).c_str(), GUI::Text{::headed(now)});
  GUI::set(page, mark.c_str(), GUI::Text{sign});
  GUI::set(
    page, mark.c_str(),
    GUI::Position{
      MARK.at.x + aside,
      MARK.at.y - Float(place - ROLL::STAFF::top()) * ROLL::STEP});
  const Flag signed_ = now.pitched && now.sign != NONE;
  GUI::set(
    page, named(row, SIGN).c_str(),
    GUI::Text{signed_ ? ROLL::STAFF::SIGN::GLYPHS[now.sign] : ""});
  ::show(page, row, HEAD, struck);
  ::show(page, row, MARK, struck && !String(sign).empty());
  ::show(page, row, SIGN, signed_);
  ::show(page, row, LEDGER, struck && ::ledgered(now.place));
  if (struck) ::paint(page, row, now, aside);
  stressed(page, row, now, aside);
}
