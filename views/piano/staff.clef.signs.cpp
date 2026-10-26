// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "../../transport.hpp"
#include "piano.paper.hpp"
#include "piano.pips.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr STRING::Hot METRE = "metre";
constexpr Float ADVANCE = 0.5172f, RISE = 0.6263f;
constexpr Float DIGIT = 2.0f * ROLL::SPACE;

}  // namespace

auto SOUND::VIEWS::ROLL::STAFF::SIGNS::named(Whole at) -> String {
  return std::format("{}.{}.{}", board(), ::METRE, at);
}

void SOUND::VIEWS::ROLL::STAFF::SIGNS::inked(
  GUI::Handle page, const String &id, STRING::Hot kind, Float depth,
  STRING::Hot ink) {
  BOARDS::place(page, board().c_str(), kind, id, {}, {});
  GUI::set(page, id.c_str(), GUI::Style{ink});
  GUI::set(page, id.c_str(), GUI::Depth{depth});
  GUI::NGA::set(page, id.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::ACROSS});
}

void SOUND::VIEWS::ROLL::STAFF::SIGNS::lettered(
  GUI::Handle page, const String &id, const STRING::Cold &text,
  const Glyph &glyph, Float down) {
  GUI::set(page, id.c_str(), GUI::Text{text});
  GUI::set(page, id.c_str(), GUI::Size{glyph.size * down});
  GUI::set(page, id.c_str(), glyph.at);
  GUI::set(page, id.c_str(), GUI::Extent{glyph.wide, glyph.size});
}

auto SOUND::VIEWS::ROLL::STAFF::SIGNS::metred(
  GUI::Handle page, Float east, const Float (&lines)[2], Float down) -> Float {
  const TRANSPORT::Section section = opening();
  const STRING::Cold digits[] = {
    std::to_string(section.numerator), std::to_string(section.denominator)};
  const Float tall = ::DIGIT / ::RISE, letter = ::ADVANCE * tall * down;
  const Float column =
    Float(std::max(digits[0].size(), digits[1].size())) * letter;
  for (Whole at = 0; at < std::size(digits); ++at) {
    const Float own = Float(digits[at].size()) * letter;
    const GUI::Position seat = {
      east + HALF * (column - own), lines[at] - BASELINE * tall};
    lettered(page, named(at), digits[at], {seat, own, tall}, down);
  }
  return column;
}
