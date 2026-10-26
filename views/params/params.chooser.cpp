// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>
#include <utility>

#include <island/input.hpp>

#include "params.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::RACK;

constexpr STRING::Hot REGION = "params";
constexpr Float FOOT = 2.0f;

Vector<String> run;
Row whose;
String claim;
Flag drawn = false;

void lower(GUI::Handle page) {
  GUI::set(page, PLATE, GUI::Visibility{false});
  ::run.clear();
  ::claim.clear();
  ::drawn = false;
}

void took(GUI::Handle page) {
  const Whole row = GUI::GET::cursor(page, WORDS);
  if (row < ::run.size())
    VIEWS::RACK::moved(
      ::whose, VIEWS::RACK::valued(VIEWS::RACK::described(::whose), row));
  ::lower(page);
}

auto elsewhere(GUI::Handle page) -> Flag {
  if (INPUT::GET::pressed(INPUT::KEYS::ESCAPE)) return true;
  for (const GUI::Event &event : GUI::GET::events(page))
    if (
      event.kind == GUI::Event::PRESSED && String(event.id) != ::claim &&
      !String(event.id).starts_with(PLATE))
      return true;
  return false;
}

}  // namespace

auto SOUND::VIEWS::RACK::choosing() -> Flag {
  return GUI::GET::visibility(VIEWS::document(), PLATE);
}

void SOUND::VIEWS::RACK::raise(
  GUI::Handle page, const String &cell, const Row &row) {
  if (choosing() && ::claim == cell) return ::lower(page);
  const AUDIO::PLUGIN::Control published = described(row);
  ::run = published.labels;
  ::whose = row;
  plate(page, ::run);
  GUI::set(page, WORDS, GUI::Cursor{stepped(published, held(row))});
  GUI::set(page, PLATE, GUI::Visibility{true});
  seated(page, cell, row);
}

void SOUND::VIEWS::RACK::seated(
  GUI::Handle page, const String &cell, const Row &row) {
  if (!choosing() || row != ::whose) return;
  ::claim = cell;
  ::drawn = true;
  const GUI::Position at = GUI::GET::origin(page, cell.c_str());
  const GUI::Position corner = GUI::GET::origin(page, ::REGION);
  const Float foot =
    at.y - corner.y + GUI::GET::measured(page, cell.c_str()).h + ::FOOT;
  const Float floor = std::max(
    0.0f,
    GUI::GET::measured(page, ::REGION).h - GUI::GET::measured(page, PLATE).h);
  GUI::set(
    page, PLATE, GUI::Position{at.x - corner.x, std::clamp(foot, 0.0f, floor)});
}

void SOUND::VIEWS::RACK::chooser() {
  const GUI::Handle page = VIEWS::document();
  if (!choosing()) return;
  if (!std::exchange(::drawn, false)) return ::lower(page);
  plate(page, ::run);
  if (GUI::GET::activated(page, WORDS)) return ::took(page);
  if (::elsewhere(page)) ::lower(page);
}

void SOUND::VIEWS::RACK::chooser(SHELL::Session &session) {
  if (!choosing()) return;
  session.print(std::format(
    "chooser {} {} rows {} cursor {}", ::whose.node, ::whose.parameter,
    ::run.size(), GUI::GET::cursor(VIEWS::document(), WORDS)));
  for (Whole step = 0; step < ::run.size(); ++step)
    session.print(std::format("step {} {}", step, ::run[step]));
}
