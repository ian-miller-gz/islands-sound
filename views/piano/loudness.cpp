// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>
#include <format>

#include <island/gui/nga.hpp>

#include "piano.loudness.hpp"
#include "piano.notation.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;
namespace LOUDNESS = SOUND::VIEWS::ROLL::LOUDNESS;

constexpr STRING::Hot DOOR = "piano.loudness.door";
constexpr STRING::Hot OPEN = "open";
constexpr STRING::Hot SHUT = "shut";
constexpr STRING::Hot LINE = "loudness {} stems {}";
constexpr STRING::Hot STEM = "stem {} note {} loud {}";
constexpr Whole SHOWN = 1, HIDDEN = 0;
constexpr Float FLAT = 1.0f;
constexpr Float ORIGIN = 0.0f;

auto opened() -> Flag { return ROLL::faced().loudness == ::SHOWN; }

auto shown(GUI::Handle page) -> Flag {
  if (GUI::GET::clicked(page, ::DOOR))
    ROLL::faced().loudness = ::opened() ? ::HIDDEN : ::SHOWN;
  const Flag open = ::opened();
  LOUDNESS::inset(page, open);
  if (GUI::GET::visibility(page, LOUDNESS::STRIP) != open)
    GUI::set(page, LOUDNESS::STRIP, GUI::Visibility{open});
  return open;
}

void viewed(GUI::Handle page) {
  const String board = ROLL::board();
  const Float run = GUI::GET::measured(page, board.c_str()).w;
  if (GUI::GET::measured(page, LOUDNESS::STRIP).w != run)
    GUI::set(page, LOUDNESS::STRIP, GUI::Extent{run, LOUDNESS::deep()});
  const Float across = GUI::NGA::GET::zoom(page, board.c_str()).value;
  const Float x = GUI::NGA::GET::pan(page, board.c_str()).x;
  const GUI::NGA::Zoom zoom = GUI::NGA::GET::zoom(page, LOUDNESS::STRIP);
  if (zoom.value != across || zoom.down != ::FLAT)
    GUI::NGA::set(page, LOUDNESS::STRIP, GUI::NGA::Zoom{across, ::FLAT});
  if (GUI::NGA::GET::pan(page, LOUDNESS::STRIP).x == x) return;
  GUI::NGA::set(
    page, LOUDNESS::STRIP, GUI::NGA::Bounds{x, x, ::ORIGIN, ::ORIGIN});
  GUI::NGA::set(page, LOUDNESS::STRIP, GUI::NGA::Pan{x, ::ORIGIN});
}

auto loud(Float velocity) -> Whole {
  return Whole(std::lround(velocity * ROLL::FULL));
}

}  // namespace

auto SOUND::VIEWS::ROLL::LOUDNESS::gathered() -> Vector<Stem> {
  Vector<Stem> stems;
  const Vector<Pip> &plates = ROLL::plates();
  for (Whole row = 0; row < plates.size(); ++row)
    if (plates[row].stock != NONE && plates[row].index != NONE)
      stems.push_back(
        {row, plates[row].stock, plates[row].index, across(plates[row].at)});
  return stems;
}

auto SOUND::VIEWS::ROLL::LOUDNESS::deep() -> Float {
  return GUI::GET::measured(document(), STRIP).h;
}

void SOUND::VIEWS::ROLL::loudness() {
  const GUI::Handle page = document();
  if (!::shown(page)) return LOUDNESS::bare(page);
  ::viewed(page);
  const Vector<LOUDNESS::Stem> stems = LOUDNESS::gathered();
  LOUDNESS::stroke(page, stems);
  LOUDNESS::seat(page, stems);
}

void SOUND::VIEWS::ROLL::loudness(SHELL::Session &session) {
  const Vector<LOUDNESS::Stem> &stems = LOUDNESS::stems();
  session.print(std::format(
    ::LINE, ::opened() ? ::OPEN : ::SHUT, ::opened() ? stems.size() : 0));
  if (!::opened()) return;
  for (Whole at = 0; at < stems.size(); ++at)
    session.print(std::format(
      ::STEM, at, stems[at].index, ::loud(LOUDNESS::velocity(stems[at]))));
}

void SOUND::VIEWS::ROLL::SHED::loudness() { LOUDNESS::bare(document()); }
