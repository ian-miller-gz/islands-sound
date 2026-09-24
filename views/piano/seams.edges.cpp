// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot INKED = "seamline";
constexpr Float HAIR = 2.0f;
constexpr Float NORTH = 0.0f;
constexpr Float HALF = 0.5f;
constexpr Float STILL = 0.0f;

struct Hair {
  Float x = 0.0f, y = 0.0f, h = 0.0f;
};

auto flushed(const Vector<Whole> &flush, Whole at) -> Flag {
  return std::find(flush.begin(), flush.end(), at) != flush.end();
}

void haired(Vector<::Hair> &hairs, Float x, Flag parted, Float scale) {
  if (!parted) return hairs.push_back({x, ::NORTH, VIEWS::ROLL::DEEP});
  const Float mark = VIEWS::ROLL::SEAMS::MARK / scale;
  const Float top = VIEWS::ROLL::DEEP * ::HALF - mark * ::HALF;
  hairs.push_back({x, ::NORTH, top});
  hairs.push_back({x, top + mark, VIEWS::ROLL::DEEP - top - mark});
}

auto haired(const Vector<Whole> &flush) -> Vector<::Hair> {
  const Vector<VIEWS::ROLL::Span> lit = VIEWS::ROLL::spans();
  const Float scale =
    VIEWS::ROLL::scaled() > ::STILL ? VIEWS::ROLL::scaled() : 1.0f;
  const Float wide = ::HAIR / scale;
  Vector<::Hair> hairs;
  for (const VIEWS::ROLL::Span &span : lit) {
    ::haired(
      hairs, VIEWS::ROLL::across(span.opens), ::flushed(flush, span.opens),
      scale);
    ::haired(
      hairs, VIEWS::ROLL::across(span.closes) - wide,
      ::flushed(flush, span.closes), scale);
  }
  return hairs;
}

}  // namespace

void SOUND::VIEWS::ROLL::SEAMS::built(
  GUI::Handle page, const String &bed, Whole count, Whole &stood,
  STRING::Hot kind, STRING::Hot dress) {
  if (stood == 0 && count != 0)
    BOARDS::place(page, VIEWS::ROLL::board().c_str(), "panel", bed, {}, {});
  BOARDS::sweep(page, bed.c_str(), count, stood);
  for (Whole row = stood; row < count; ++row) {
    const String id = GUI::SAC::SEAT::named(bed.c_str(), row);
    BOARDS::place(page, bed.c_str(), kind, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{dress});
    GUI::set(page, id.c_str(), GUI::Depth{VIEWS::ROLL::CREST});
  }
  stood = count;
}

void SOUND::VIEWS::ROLL::SEAMS::bared(
  GUI::Handle page, const String &bed, Whole &stood) {
  if (stood == 0) return;
  BOARDS::sweep(page, bed.c_str(), 0, stood);
  BOARDS::drop(page, bed);
  stood = 0;
}

void SOUND::VIEWS::ROLL::SEAMS::lined(
  GUI::Handle page, const Vector<Whole> &flush, Whole &stood) {
  const String bed = VIEWS::ROLL::board() + "." + SEAMS::EDGE;
  const Vector<::Hair> hairs = ::haired(flush);
  built(page, bed, hairs.size(), stood, "panel", ::INKED);
  const Float scale = VIEWS::ROLL::scaled();
  const Float wide = scale > ::STILL ? ::HAIR / scale : ::HAIR;
  for (Whole hair = 0; hair < hairs.size(); ++hair) {
    const String id = GUI::SAC::SEAT::named(bed.c_str(), hair);
    GUI::set(page, id.c_str(), GUI::Position{hairs[hair].x, hairs[hair].y});
    GUI::set(page, id.c_str(), GUI::Extent{wide, hairs[hair].h});
  }
}
