// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/seat.hpp>

#include "piano.seams.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot GLYPH = "seamcross";
constexpr STRING::Hot CROSS = "✖";
constexpr Float LETTER = 26.0f;
constexpr Float HALF = 0.5f;
constexpr Float STILL = 0.0f;

}  // namespace

void SOUND::VIEWS::ROLL::SEAMS::crossed(
  GUI::Handle page, const String &bed, const Vector<Seam> &seams,
  Whole &stood) {
  built(page, bed, seams.size(), stood, "button", ::GLYPH);
  const Float scale = scaled();
  const Float wide = scale > ::STILL ? MARK / scale : MARK;
  const Float top = ROWS::deep() * ::HALF - MARK * ::HALF;
  for (Whole seam = 0; seam < seams.size(); ++seam) {
    const String id = GUI::SAC::SEAT::named(bed.c_str(), seam);
    GUI::set(page, id.c_str(), GUI::Text{String(::CROSS)});
    GUI::set(page, id.c_str(), GUI::Size{::LETTER});
    GUI::set(
      page, id.c_str(),
      GUI::Position{across(seams[seam].at) - wide * ::HALF, top});
    GUI::set(page, id.c_str(), GUI::Extent{wide, MARK});
  }
}
