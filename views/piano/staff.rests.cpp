// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../boards.hpp"
#include "piano.beams.hpp"
#include "piano.notation.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr STRING::Hot NAME = "rest", INK = "staffstem", LABEL = "label";
constexpr STRING::Hot RESTS[] = {"\U0001D13D", "\U0001D13E", "\U0001D13F"};
constexpr Integer MIDDLE = 4;

struct Ink {
  Float east = 0.0f, across = 0.0f, rise = 0.0f;
};
constexpr Ink INKS[] = {
  {0.307f, 0.179f, 0.5005f},
  {0.312f, 0.181f, 0.4995f},
  {0.38f, 0.215f, 0.4995f}};

VIEWS::Pool pool;
Vector<STAFF::Rest> kept;
STAFF::Scale held;

auto named(Whole at) -> String {
  return std::format("{}.{}.{}", ROLL::board(), ::NAME, at);
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  for (Whole at = count; at < ::pool.stood; ++at)
    BOARDS::drop(page, ::named(at));
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::named(at);
    BOARDS::place(page, ROLL::board().c_str(), ::LABEL, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::INK});
  }
  VIEWS::built(::pool, count);
}

void seat(Whole at, const STAFF::Rest &rest, STAFF::Scale scale) {
  const GUI::Handle page = VIEWS::document();
  const String id = ::named(at);
  const Ink &ink = ::INKS[rest.beams];
  const Float size = ROLL::EM * scale.across, tall = ROLL::EM * scale.down;
  GUI::set(page, id.c_str(), GUI::Text{::RESTS[rest.beams]});
  GUI::set(
    page, id.c_str(),
    GUI::Position{
      ROLL::across(rest.at) + ROLL::HALF * ROLL::SPACE - ink.across * size,
      STAFF::centred(::MIDDLE) - (ROLL::BASELINE - ink.rise) * tall});
  GUI::set(page, id.c_str(), GUI::Size{size});
  GUI::set(page, id.c_str(), GUI::Extent{ink.east * size, tall});
}

auto rescaled(STAFF::Scale scale) -> Flag {
  const Flag moved = scale.across != ::held.across || scale.down != ::held.down;
  ::held = scale;
  return moved;
}

}  // namespace

void SOUND::VIEWS::ROLL::STAFF::rested(Flag moved) {
  const Flag fresh = VIEWS::reborn(::pool);
  const Scale scale = scaled();
  if (!::rescaled(scale) && !fresh && !moved) return;
  ::kept = ROLL::GET::presentation() == ROLL::PRESENTATION::GRID
             ? Vector<Rest>{}
             : rested(GET::stems());
  ::build(VIEWS::sized(::pool, ::kept.size(), window().w));
  VIEWS::light(::pool, ::named, ::kept.size());
  for (Whole at = 0; at < ::kept.size(); ++at) ::seat(at, ::kept[at], scale);
}

void SOUND::VIEWS::ROLL::STAFF::SHED::rested() {
  VIEWS::reborn(::pool);
  ::kept.clear();
  ::build(0);
}
