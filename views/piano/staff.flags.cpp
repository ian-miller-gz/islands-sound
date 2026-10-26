// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../boards.hpp"
#include "piano.beams.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr STRING::Hot NAME = "flag", INK = "staffstem";
constexpr STRING::Hot LABEL = "label", STROKE = "stroke";
VIEWS::Pool up, down;

auto hung(Whole at) -> String {
  return std::format("{}.{}.{}", ROLL::board(), ::NAME, at);
}

auto bowed(Whole at) -> String {
  return std::format(
    "{}.{}.{}.{}", ROLL::board(), ::NAME, at / STAFF::DEPTH, at % STAFF::DEPTH);
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

void build(Whole stems) {
  VIEWS::reborn(::up);
  VIEWS::reborn(::down);
  const Float seen = ROLL::window().w;
  ::build(::up, ::hung, ::LABEL, VIEWS::sized(::up, stems, seen));
  ::build(
    ::down, ::bowed, ::STROKE,
    VIEWS::sized(::down, stems * STAFF::DEPTH, seen));
}

auto counted(const Vector<STAFF::Beam> &beams, Whole stems) -> Vector<Whole> {
  Vector<Whole> flags(stems, 0);
  for (const STAFF::Beam &beam : beams)
    if (beam.to == NONE) ++flags[beam.stem];
  return flags;
}

auto light(const String &id, Flag lit) -> Flag {
  GUI::set(VIEWS::document(), id.c_str(), GUI::Visibility{lit});
  if (!lit) GUI::set(VIEWS::document(), id.c_str(), GUI::Text{""});
  if (!lit) STAFF::cut(id, STAFF::Ends{});
  return lit;
}

void hang(const Vector<STAFF::Stem> &stems, const Vector<Whole> &flags) {
  const STAFF::Scale scale = STAFF::scaled();
  for (Whole at = 0; at < ::up.stood; ++at) {
    const Flag lit = at < stems.size() && !stems[at].foot && flags[at] > 0;
    if (::light(::hung(at), lit)) STAFF::hang(::hung(at), stems[at], scale);
  }
}

void bow(const Vector<STAFF::Stem> &stems, const Vector<Whole> &flags) {
  const STAFF::Scale scale = STAFF::scaled();
  for (Whole at = 0; at < ::down.stood; ++at) {
    const Whole stem = at / STAFF::DEPTH, rank = at % STAFF::DEPTH;
    const Flag lit =
      stem < stems.size() && stems[stem].foot && rank < flags[stem];
    if (::light(::bowed(at), lit))
      STAFF::bow(::bowed(at), stems[stem], rank, scale);
  }
}

}  // namespace

void SOUND::VIEWS::ROLL::STAFF::flagged(const Vector<Beam> &beams) {
  const Vector<Stem> &stems = GET::stems();
  ::build(stems.size());
  const Vector<Whole> flags = ::counted(beams, stems.size());
  ::hang(stems, flags);
  ::bow(stems, flags);
}

void SOUND::VIEWS::ROLL::STAFF::SHED::flagged() { ::build(0); }
