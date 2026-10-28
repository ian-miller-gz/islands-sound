// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <iterator>

#include "../../transport.hpp"

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

// The pitched staff says its key and its rules, then only the lines the
// signature spells plain inside the grand staff (a line a pitch would be
// a hundred and twenty-eight).
static void pitched(SHELL::Session &session) {
  const Key key = TRANSPORT::keyed(0);
  session.print(std::format(
    "staff pitched tonic {} scale {} top {} rules {}", key.tonic,
    key.scale.empty() ? String("none") : key.scale, STAFF::top(),
    STAFF::GET::rules().size()));
  for (const STAFF::Line &line : STAFF::GET::lines())
    if (
      line.place >= STAFF::GRAND[0] && line.place <= std::rbegin(STAFF::GRAND)[0] &&
      STAFF::accidental(line.note) == NONE)
      session.print(
        std::format("line {} note {} {}", line.place, line.note, line.name));
}

void SOUND::VIEWS::ROLL::STAFF::lined(SHELL::Session &session) {
  if (!ROLL::GET::staffed()) return;
  const GUI::Extent extent = spanned();
  session.print(std::format("staff top {:g} span {:g}", extent.w, extent.h));
  if (ROLL::GET::pitched()) {
    ::pitched(session);
    ::stems(session);
    return;
  }
  session.print(std::format("lines {}", GET::lines().size()));
  session.print(std::format("heads side {:g}", HEADS::side(ROLL::pips())));
  for (const Line &line : GET::lines())
    session.print(
      std::format("line {} note {} {}", line.place, line.note, line.name));
  ::stems(session);
}
