// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <utility>

#include "../../boards.hpp"
#include "piano.beams.hpp"
#include "piano.internal.hpp"
#include "piano.weights.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;
namespace WEIGHTS = SOUND::VIEWS::ROLL::WEIGHTS;

constexpr STRING::Hot UPRIGHT = "stem";
constexpr STRING::Hot ACROSS = "beam";
constexpr STRING::Hot INK = "staffstem";
constexpr STRING::Hot PANEL = "panel", STROKE = "stroke";
VIEWS::Pool upright, across;
Vector<STAFF::Beam> kept;
Vector<Float> weights;

auto stem(Whole at) -> String {
  return std::format("{}.{}.{}", ROLL::board(), ::UPRIGHT, at);
}

auto beam(Whole at) -> String {
  return std::format(
    "{}.{}.{}.{}", ROLL::board(), ::ACROSS, at / STAFF::DEPTH,
    at % STAFF::DEPTH);
}

void build(
  VIEWS::Pool &pool, VIEWS::Named named, STRING::Hot kind, Whole count) {
  const GUI::Handle page = VIEWS::document();
  for (Whole at = count; at < pool.stood; ++at) BOARDS::drop(page, named(at));
  for (Whole at = pool.stood; at < count; ++at) {
    const String id = named(at);
    BOARDS::place(page, ROLL::board().c_str(), kind, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::INK});
  }
  VIEWS::built(pool, count);
}

auto slot(const STAFF::Beam &beam) -> Whole {
  return beam.stem * STAFF::DEPTH + beam.rank;
}

void light(const Vector<STAFF::Beam> &beams) {
  const GUI::Handle page = VIEWS::document();
  Vector<Flag> lit(::across.stood, false);
  for (const STAFF::Beam &beam : beams) lit[::slot(beam)] = beam.to != NONE;
  for (Whole at = 0; at < lit.size(); ++at) {
    GUI::set(page, ::beam(at).c_str(), GUI::Visibility{lit[at]});
    if (!lit[at]) STAFF::cut(::beam(at), STAFF::Ends{});
  }
}

auto weighed() -> Flag {
  const Vector<Float> now = {
    WEIGHTS::drawn(WEIGHTS::Across{WEIGHTS::STEM}),
    WEIGHTS::drawn(WEIGHTS::Down{WEIGHTS::BEAM}),
    ROLL::HEADS::side(ROLL::plates())};
  return now != std::exchange(::weights, now);
}

}  // namespace

void SOUND::VIEWS::ROLL::STAFF::beamed(Flag moved) {
  rested(moved);
  const Flag fresh = VIEWS::reborn(::upright);
  const Flag reweighed = ::weighed();
  if (!VIEWS::reborn(::across) && !fresh && !moved && !reweighed) return;
  const Vector<Stem> &stems = GET::stems();
  ::kept = joined(stems);
  const Float seen = window().w;
  ::build(
    ::upright, ::stem, ::PANEL, VIEWS::sized(::upright, stems.size(), seen));
  ::build(
    ::across, ::beam, ::STROKE,
    VIEWS::sized(::across, stems.size() * STAFF::DEPTH, seen));
  VIEWS::light(::upright, ::stem, stems.size());
  ::light(::kept);
  for (Whole at = 0; at < stems.size(); ++at) cut(::stem(at), boxed(stems[at]));
  for (const Beam &beam : ::kept)
    if (beam.to != NONE) cut(::beam(::slot(beam)), ended(beam, stems));
  flagged(::kept);
}

void SOUND::VIEWS::ROLL::STAFF::SHED::beamed() {
  VIEWS::reborn(::upright);
  VIEWS::reborn(::across);
  ::kept.clear();
  ::build(::upright, ::stem, ::PANEL, 0);
  ::build(::across, ::beam, ::STROKE, 0);
  flagged();
  rested();
}
