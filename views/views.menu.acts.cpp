// SPDX-License-Identifier: AGPL-3.0-or-later
#include <charconv>

#include <island/gui/nga.hpp>

#include "../boards.hpp"
#include "views.menu.acts.hpp"

namespace {
using namespace SOUND;
using VIEWS::MENU::Act;

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
  const String head = String(list) + ".";
  if (!ask.target.starts_with(head)) return NONE;
  const Whole n = ::numbered(ask.target.substr(head.size()));
  return n == NONE ? NONE : GUI::GET::first(page, list) + n;
}

auto SOUND::VIEWS::MENU::offer(
  GUI::Handle page, STRING::Hot list, const Vector<Act> &acts) -> Flag {
  if (::took(list, acts)) return true;
  const Whole row = asked(page, list);
  if (row == NONE) return false;
  const Vector<Whole> run = ::shown(acts, row);
  if (run.empty()) return false;
  Vector<String> words;
  for (const Whole at : run) words.push_back(String(acts[at].word));
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(page);
  ::raised = {.list = list, .row = row, .run = run};
  raise(words, ask.x, ask.y, list);
  return false;
}
