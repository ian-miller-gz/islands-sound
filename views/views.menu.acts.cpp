// SPDX-License-Identifier: AGPL-3.0-or-later
#include <charconv>

#include <island/gui/nga.hpp>

#include "../boards.hpp"
#include "views.menu.acts.hpp"

namespace {
using namespace SOUND;
using VIEWS::MENU::Act;

constexpr Whole SUMMON = 10;
constexpr Float GAP = 4.0f;
constexpr VIEWS::Chord KEYED = {
  .action = INPUT::KEYS::HOTKEY, .code = ::SUMMON, .shift = true};

struct Raised {
  String list;
  Whole row = NONE;
  Vector<Whole> run;
};
Raised raised;

auto numbered(const String &rest) -> Whole {
  Whole n = NONE;
  const char *end = rest.data() + rest.size();
  const auto [stop, fault] = std::from_chars(rest.data(), end, n);
  if (fault != std::errc{} || stop == rest.data()) return NONE;
  return stop == end || *stop == '.' ? n : NONE;
}

auto under(const String &target, STRING::Hot list) -> Whole {
  const String head = String(list) + ".";
  if (!target.starts_with(head)) return NONE;
  return ::numbered(target.substr(head.size()));
}

void hang(GUI::Handle page, const Vector<String> &labels, STRING::Hot list) {
  const String door = VIEWS::MENU::GET::door();
  const GUI::Position at = GUI::GET::origin(page, door.c_str());
  const GUI::Extent size = GUI::GET::measured(page, door.c_str());
  VIEWS::MENU::raise(labels, at.x, at.y + size.h + ::GAP, list);
}

auto shown(const Vector<Act> &acts, Whole row) -> Vector<Whole> {
  Vector<Whole> run;
  for (Whole at = 0; at < acts.size(); ++at)
    if (acts[at].shown == nullptr || acts[at].shown(row)) run.push_back(at);
  return run;
}

auto took(STRING::Hot list, const Vector<Act> &acts) -> Flag {
  if (::raised.list != list) return false;
  const Whole taken = VIEWS::MENU::taken(list);
  const Raised held = ::raised;
  if (taken == NONE && VIEWS::MENU::standing()) return false;
  ::raised = {};
  if (taken == NONE || taken >= held.run.size()) return false;
  const Act &act = acts[held.run[taken]];
  if (act.act != nullptr) act.act(held.row);
  return true;
}

}  // namespace

auto SOUND::VIEWS::MENU::asked(GUI::Handle page, STRING::Hot list) -> Whole {
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(page);
  if (!ask.asked || !ask.board.empty()) return NONE;
  const Whole n = ::under(ask.target, list);
  return n == NONE ? NONE : GUI::GET::first(page, list) + n;
}

auto SOUND::VIEWS::MENU::keyed(GUI::Handle page, STRING::Hot list) -> Whole {
  const String cell = GUI::GET::editing(page);
  const Whole n = ::under(cell, list);
  if (n == NONE || GUI::GET::seated(page)) return NONE;
  if (!VIEWS::chorded(::KEYED)) return NONE;
  park(page, cell);
  return GUI::GET::first(page, list) + n;
}

auto SOUND::VIEWS::MENU::offer(
  GUI::Handle page, STRING::Hot list, const Vector<Act> &acts) -> Flag {
  if (::took(list, acts)) return true;
  const Whole asking = asked(page, list);
  const Whole row = asking == NONE ? keyed(page, list) : asking;
  if (row == NONE) return false;
  const Vector<Whole> run = ::shown(acts, row);
  if (run.empty()) return false;
  Vector<String> labels;
  for (const Whole at : run) labels.push_back(String(acts[at].word));
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(page);
  ::raised = {.list = list, .row = row, .run = run};
  if (asking == NONE) {
    ::hang(page, labels, list);
    return false;
  }
  raise(labels, ask.x, ask.y, list);
  return false;
}
