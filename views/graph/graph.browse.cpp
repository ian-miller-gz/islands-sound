// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <island/midi.hpp>
#include <format>

#include <island/input.hpp>

#include "../../commands.hpp"
#include "../../control.hpp"
#include "../../graph.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "graph.browse.internal.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;
namespace BROWSE = SOUND::VIEWS::WIRED::BROWSE;

String filing;

void took(GUI::Handle page, const Vector<BROWSE::Offer> &rows) {
  const Whole row = GUI::GET::cursor(page, BROWSE::ROWS);
  if (row >= rows.size()) return;
  VIEWS::WIRED::raise(false);
  const BROWSE::Offer &taken = rows[row];
  if (taken.from == BROWSE::LANES)
    return void(
      COMMANDS::laned(VIEWS::WIRED::steered(), KIND::meant(taken.name)));
  if (BROWSE::devised(taken.from))
    return VIEWS::WIRED::surface(
      BROWSE::surfacing(taken.from), taken.device,
      taken.lane == NONE ? 0 : taken.lane);
  if (!taken.target.empty()) return VIEWS::WIRED::send(taken.target);
  if (!taken.among.empty())
    return taken.lane == NONE ? CONTROL::choose(taken.name)
                              : CONTROL::choose(taken.lane);
  VIEWS::WIRED::take(taken.name);
}

}  // namespace

void SOUND::VIEWS::WIRED::raise(Flag on) {
  ::filing.clear();
  GUI::set(document(), BROWSE::PANEL, GUI::Visibility{on});
}

void SOUND::VIEWS::WIRED::raise(const String &filing) {
  ::filing = filing;
  GUI::set(document(), BROWSE::PANEL, GUI::Visibility{true});
}

auto SOUND::VIEWS::WIRED::browsing() -> Flag {
  return GUI::GET::visibility(document(), BROWSE::PANEL);
}

auto SOUND::VIEWS::WIRED::filed() -> String {
  return browsing() ? ::filing : String();
}

void SOUND::VIEWS::WIRED::browse() {
  const GUI::Handle page = document();
  const Vector<BROWSE::Offer> rows = BROWSE::rows(::filing);
  listing(page, rows);
  if (!browsing()) return;
  if (GUI::GET::activated(page, BROWSE::ROWS)) return ::took(page, rows);
  if (VIEWS::elsewhere(DOORS, BROWSE::PANEL)) raise(false);
}

void SOUND::VIEWS::WIRED::say(SHELL::Session &session) {
  if (!browsing()) return;
  const Vector<BROWSE::Offer> rows = BROWSE::rows(::filing);
  session.print(std::format(
    "browse rows {} cursor {}{}", rows.size(),
    GUI::GET::cursor(document(), BROWSE::ROWS),
    ::filing.empty() ? String() : " filed " + ::filing));
  for (const BROWSE::Offer &row : rows)
    session.print(
      row.reading.empty()
        ? std::format(
            "offer {} takes {} gives {} among {}", row.name,
            BROWSE::spelled(row.takes), BROWSE::spelled(row.gives),
            BROWSE::filed(row))
        : std::format(
            "offer {} {} among {}", row.name, row.reading, BROWSE::filed(row)));
}
