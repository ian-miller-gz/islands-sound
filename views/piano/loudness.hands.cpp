// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>

#include <island/gui/nga.hpp>

#include "piano.loudness.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace LOUDNESS = SOUND::VIEWS::ROLL::LOUDNESS;

constexpr Float GRASP = 10.0f;
constexpr Float WHOLE = 1.0f;

LOUDNESS::Hold hold;

auto boarded(GUI::Handle page, Float face) -> Float {
  const Float scale = GUI::NGA::GET::zoom(page, ROLL::board().c_str()).value;
  return scale > 0.0f ? face / scale : face;
}

auto grasped(GUI::Handle page, const Vector<LOUDNESS::Stem> &stems, Float x)
  -> const LOUDNESS::Stem * {
  const Vector<Whole> chosen = ROLL::chosen(page);
  const auto apart = [x, &chosen](const LOUDNESS::Stem &stem) {
    const Flag loose = std::ranges::find(chosen, stem.row) == chosen.end();
    return std::pair{std::abs(stem.across - x), loose};
  };
  const auto best = std::ranges::min_element(stems, {}, apart);
  if (best == stems.end() || apart(*best).first > ::boarded(page, ::GRASP))
    return nullptr;
  return &*best;
}

void painted(
  GUI::Handle page, const Vector<LOUDNESS::Stem> &stems, Float x, Float y) {
  const Float edge = ::boarded(page, LOUDNESS::WIDE) * ROLL::HALF;
  const Float low = std::min(::hold.x, x) - edge;
  const Float high = std::max(::hold.x, x) + edge;
  const Float run = x - ::hold.x;
  for (const LOUDNESS::Stem &stem : stems) {
    if (stem.across < low || stem.across > high) continue;
    const Float share = run == 0.0f ? ::WHOLE : (stem.across - ::hold.x) / run;
    const Float at =
      ::hold.y + std::clamp(share, 0.0f, ::WHOLE) * (y - ::hold.y);
    LOUDNESS::touch(::hold, stem.stock, stem.index).velocity =
      LOUDNESS::clamped((LOUDNESS::deep() - at) / LOUDNESS::deep());
  }
  ::hold.x = x;
  ::hold.y = y;
}

void opened(
  GUI::Handle page, const Vector<LOUDNESS::Stem> &stems,
  const GUI::NGA::Ask &at) {
  ::hold = {.standing = true, .x = at.x, .y = at.y};
  const LOUDNESS::Stem *stem = ::grasped(page, stems, at.x);
  ::hold.relative = stem != nullptr;
  if (stem == nullptr) return ::painted(page, stems, at.x, at.y);
  LOUDNESS::touch(::hold, stem->stock, stem->index);
  const Vector<Whole> chosen = ROLL::chosen(page);
  if (std::ranges::find(chosen, stem->row) == chosen.end()) return;
  const Vector<ROLL::Pip> &plates = ROLL::plates();
  for (const Whole row : chosen)
    if (row < plates.size() && plates[row].stock != NONE)
      LOUDNESS::touch(::hold, plates[row].stock, plates[row].index);
}

void moved(
  GUI::Handle page, const Vector<LOUDNESS::Stem> &stems,
  const GUI::NGA::Ask &at) {
  if (!::hold.relative) return ::painted(page, stems, at.x, at.y);
  const Float travel = (::hold.y - at.y) / LOUDNESS::deep();
  for (LOUDNESS::Touch &one : ::hold.touched)
    one.velocity = LOUDNESS::clamped(one.from + travel);
}

}  // namespace

void SOUND::VIEWS::ROLL::LOUDNESS::stroke(
  GUI::Handle page, const Vector<Stem> &stems) {
  const GUI::NGA::Ask press = GUI::NGA::GET::pressed(page, STRIP);
  if (!press.board.empty()) ::opened(page, stems, press);
  const GUI::NGA::Ask held = GUI::NGA::GET::stroked(page, STRIP);
  if (!held.board.empty() && !::hold.standing) ::opened(page, stems, held);
  if (!held.board.empty()) ::moved(page, stems, held);
  const GUI::NGA::Ask lift = GUI::NGA::GET::lifted(page, STRIP);
  if (lift.board.empty() || !::hold.standing) return;
  ::moved(page, stems, lift);
  wrote(::hold);
  ::hold = {};
}

auto SOUND::VIEWS::ROLL::LOUDNESS::velocity(const Stem &stem) -> Float {
  for (const Touch &one : ::hold.touched)
    if (one.stock == stem.stock && one.index == stem.index) return one.velocity;
  return scored(stem.stock, stem.index);
}
