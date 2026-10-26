// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../graph.hpp"
#include "graph.browse.crop.hpp"
#include "graph.browse.internal.hpp"
#include "graph.keys.hpp"

namespace {
using namespace SOUND;
namespace BROWSE = SOUND::VIEWS::WIRED::BROWSE;

Map<GUI::Handle, String> filing;

auto said(const BROWSE::Offer &row) -> String {
  if (row.directory)
    return std::format("directory {} holds {}", row.name, row.holds);
  if (!row.reading.empty())
    return std::format(
      "offer {} {} among {}", row.name, row.reading, BROWSE::filed(row));
  return std::format(
    "offer {} takes {} gives {} among {}", row.name, BROWSE::spelled(row.takes),
    BROWSE::spelled(row.gives), BROWSE::filed(row));
}

}  // namespace

void SOUND::VIEWS::WIRED::BROWSE::enter(const String &path) {
  GUI::set(VIEWS::document(), ROWS, GUI::Cursor{0});
  VIEWS::WIRED::raise(path);
}

void SOUND::VIEWS::WIRED::raise(Flag on) {
  BROWSE::seat(document(), on);
  ::filing[document()].clear();
  BROWSE::forget();
  GUI::set(document(), BROWSE::PANEL, GUI::Visibility{on});
}

void SOUND::VIEWS::WIRED::raise(const String &filing) {
  BROWSE::seat(document(), true);
  ::filing[document()] = filing;
  BROWSE::forget();
  GUI::set(document(), BROWSE::PANEL, GUI::Visibility{true});
}

auto SOUND::VIEWS::WIRED::browsing() -> Flag {
  return GUI::GET::visibility(document(), BROWSE::PANEL);
}

auto SOUND::VIEWS::WIRED::filed() -> String {
  return browsing() ? ::filing[document()] : String();
}

void SOUND::VIEWS::WIRED::browse() {
  keys();
  const GUI::Handle page = document();
  const String filing = ::filing[page];
  const Vector<BROWSE::Offer> rows = BROWSE::shown(filing);
  listing(page, rows);
  if (!browsing()) return;
  BROWSE::head(page, filing);
  BROWSE::sought(page);
  if (GUI::GET::clicked(page, BROWSE::UP))
    return BROWSE::enter(BROWSE::up(filing));
  if (GUI::GET::activated(page, BROWSE::ROWS)) return BROWSE::took(page, rows);
  if (VIEWS::elsewhere(DOORS, BROWSE::PANEL)) raise(false);
}

void SOUND::VIEWS::WIRED::say(SHELL::Session &session) {
  if (!browsing()) return;
  const String filing = ::filing[document()];
  const String run = GUI::GET::text(document(), BROWSE::FIND);
  const Vector<BROWSE::Offer> rows = BROWSE::shown(filing);
  session.print(std::format(
    "browse rows {} cursor {}{}{}", rows.size(),
    GUI::GET::cursor(document(), BROWSE::ROWS),
    filing.empty() ? String() : " filed " + filing,
    run.empty() ? String() : " sought " + run));
  for (const BROWSE::Offer &row : rows) session.print(::said(row));
  BROWSE::tell(session, document());
}
