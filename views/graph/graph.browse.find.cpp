// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "graph.browse.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::WIRED::BROWSE;
namespace WIRED = SOUND::VIEWS::WIRED;

constexpr STRING::Hot ROSTERS[] = {WIRED::TRACKS,  WIRED::BUSES, WIRED::INPUTS,
                                   WIRED::SOURCES, WIRED::SINKS, WIRED::NOTED};

auto folded(const String &text) -> String {
  String said;
  for (const unsigned char letter : text)
    said += static_cast<char>(VIEWS::folded(letter));
  return said;
}

void clear(GUI::Handle page) {
  GUI::set(page, FIND, GUI::Text{String()});
  GUI::set(page, FIND, GUI::Caret{});
  GUI::set(page, ROWS, GUI::Cursor{0});
}

auto pooled() -> Vector<Offer> {
  Vector<Offer> pool = catalogued();
  for (const STRING::Hot roster : ::ROSTERS) {
    const Vector<Offer> rows = rostered(roster);
    pool.insert(pool.end(), rows.begin(), rows.end());
  }
  return pool;
}

}  // namespace

auto SOUND::VIEWS::WIRED::BROWSE::found(const String &run) -> Vector<Offer> {
  const String sought = ::folded(run);
  Vector<Offer> exact, begun, held;
  for (const Offer &row : ::pooled()) {
    const String name = ::folded(row.name);
    const Whole at = name.find(sought);
    if (at == String::npos) continue;
    (name == sought ? exact : at == 0 ? begun : held).push_back(row);
  }
  for (Vector<Offer> *part : {&exact, &begun, &held})
    std::sort(part->begin(), part->end(), before);
  exact.insert(exact.end(), begun.begin(), begun.end());
  exact.insert(exact.end(), held.begin(), held.end());
  return exact;
}

auto SOUND::VIEWS::WIRED::BROWSE::shown(const String &filing) -> Vector<Offer> {
  const String run = GUI::GET::text(VIEWS::document(), FIND);
  return run.empty() ? rows(filing) : found(run);
}

void SOUND::VIEWS::WIRED::BROWSE::sought(GUI::Handle page) {
  if (GUI::GET::edited(page, FIND)) GUI::set(page, ROWS, GUI::Cursor{0});
  if (GUI::GET::committed(page, FIND))
    return took(page, shown(VIEWS::WIRED::filed()));
  if (!GUI::GET::text(page, FIND).empty() && VIEWS::escaped()) ::clear(page);
}

void SOUND::VIEWS::WIRED::BROWSE::seat(GUI::Handle page, Flag on) {
  if (!GUI::GET::text(page, FIND).empty()) ::clear(page);
  if (on && !VIEWS::WIRED::browsing()) GUI::edit(page, FIND);
  if (!on && GUI::GET::editing(page) == FIND) GUI::edit(page, "");
}
