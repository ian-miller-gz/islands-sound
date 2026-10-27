// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "piano.notation.hpp"
#include "piano.beams.hpp"

namespace {
using namespace SOUND;
namespace STAFF = SOUND::VIEWS::ROLL::STAFF;

constexpr STRING::Hot WAYS[] = {"up", "down"};

void stems(SHELL::Session &session) {
  session.print(std::format(
    "stems {} beams {}", STAFF::GET::stems().size(),
    STAFF::joined(STAFF::GET::stems()).size()));
  session.print(
    std::format("rests {}", STAFF::rested(STAFF::GET::stems()).size()));
  for (const STAFF::Stem &stem : STAFF::GET::stems())
    session.print(
      std::format("stem {} {} {}", stem.at, ::WAYS[stem.foot], stem.beams));
  for (const STAFF::Slope &slope : STAFF::GET::slopes())
    session.print(std::format("slope {} {}", slope.at, slope.quarters));
}

}  // namespace

void SOUND::VIEWS::ROLL::STAFF::lined(SHELL::Session &session) {
  if (ROLL::GET::notation() != PERCUSSION) return;
  const GUI::Extent extent = spanned();
  session.print(std::format("staff top {:g} span {:g}", extent.w, extent.h));
  session.print(std::format("lines {}", GET::lines().size()));
  session.print(std::format("heads side {:g}", HEADS::side(ROLL::pips())));
  for (const Line &line : GET::lines())
    session.print(
      std::format("line {} note {} {}", line.place, line.note, line.name));
  ::stems(session);
}
