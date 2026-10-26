// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "piano.loudness.hpp"
#include "piano.pips.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace LOUDNESS = SOUND::VIEWS::ROLL::LOUDNESS;

constexpr STRING::Hot KIND = "panel";
constexpr STRING::Hot NOTE = "note";
constexpr STRING::Hot CHOSEN = "staffchosen";
constexpr Float ORIGIN = 0.0f;

struct Worn {
  Whole tier = NONE;
  Flag chosen = false;
  auto operator==(const Worn &) const -> Bool = default;
};

VIEWS::Pool pool;
Vector<Worn> worn;
Vector<LOUDNESS::Stem> shown;

auto named(Whole at) -> String {
  return GUI::SAC::SEAT::named(LOUDNESS::STEMS, at);
}

void built(GUI::Handle page, Whole count) {
  BOARDS::sweep(page, LOUDNESS::STEMS, count, ::pool.stood);
  for (Whole at = ::pool.stood; at < count; ++at)
    BOARDS::place(page, LOUDNESS::STRIP, ::KIND, ::named(at), {}, {});
  VIEWS::built(::pool, count);
  ::worn.resize(count);
}

void dressed(GUI::Handle page, Whole at, const Worn &now) {
  if (::worn[at] == now) return;
  const String id = ::named(at);
  const String tint = ROLL::tinted(::NOTE, now.tier);
  GUI::set(page, id.c_str(), GUI::Style{tint.c_str()});
  if (now.chosen) GUI::set(page, id.c_str(), GUI::Style{::CHOSEN});
  ::worn[at] = now;
}

void placed(
  GUI::Handle page, Whole at, const LOUDNESS::Stem &stem, Float wide,
  Float deep) {
  const String id = ::named(at);
  const Float tall = LOUDNESS::velocity(stem) * deep;
  GUI::set(page, id.c_str(), GUI::Position{stem.across, deep - tall});
  GUI::set(page, id.c_str(), GUI::Extent{wide, tall});
}

}  // namespace

void SOUND::VIEWS::ROLL::LOUDNESS::seat(
  GUI::Handle page, const Vector<Stem> &stems) {
  if (VIEWS::reborn(::pool)) ::worn.clear();
  const Whole seats =
    VIEWS::sized(::pool, stems.size(), GUI::GET::measured(page, STRIP).w);
  if (seats != ::pool.stood) ::built(page, seats);
  VIEWS::light(::pool, ::named, stems.size());
  const Float scale = GUI::NGA::GET::zoom(page, STRIP).value;
  const Float wide = scale > ::ORIGIN ? WIDE / scale : WIDE;
  const Vector<Whole> chosen = ROLL::chosen(page);
  const Vector<Pip> &plates = ROLL::plates();
  for (Whole at = 0; at < stems.size(); ++at) {
    const Pip &pip = plates[stems[at].row];
    ::dressed(
      page, at,
      {tier(pip), std::ranges::find(chosen, stems[at].row) != chosen.end()});
    ::placed(page, at, stems[at], wide, deep());
  }
  ::shown = stems;
}

void SOUND::VIEWS::ROLL::LOUDNESS::bare(GUI::Handle page) {
  if (VIEWS::reborn(::pool)) ::worn.clear();
  if (::pool.stood != 0) ::built(page, 0);
  ::shown.clear();
}

auto SOUND::VIEWS::ROLL::LOUDNESS::stems() -> const Vector<Stem> & {
  return ::shown;
}
