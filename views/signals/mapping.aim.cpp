// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../graph.hpp"
#include "signals.mapping.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot BUTTON = "lane.map";
constexpr STRING::Hot MEANT = "lane.number.name";
constexpr STRING::Hot PANEL = "lane.mapping";
constexpr STRING::Hot ROWS = "lane.mapping.rows";

Vector<VIEWS::SIGNALS::Offer> rows;
String node;

auto nodes() -> Vector<VIEWS::SIGNALS::Offer> {
  Vector<VIEWS::SIGNALS::Offer> run;
  for (const Node &seated : GRAPH::held().nodes)
    run.push_back(
      {.name = seated.name, .words = VIEWS::parameters(seated.name)});
  return run;
}

auto parameters(const String &name) -> Vector<VIEWS::SIGNALS::Offer> {
  Vector<VIEWS::SIGNALS::Offer> run;
  for (Whole row = 0; row < VIEWS::parameters(name); ++row)
    run.push_back({.name = VIEWS::named(name, row)});
  return run;
}

void spread(GUI::Handle page, Flag on) {
  ::node.clear();
  ::rows = on ? ::nodes() : Vector<VIEWS::SIGNALS::Offer>{};
  GUI::set(page, ::PANEL, GUI::Visibility{on});
  if (on) VIEWS::SIGNALS::hang(::rows);
}

void browse(GUI::Handle page, const String &name) {
  ::node = name;
  ::rows = ::parameters(name);
  GUI::set(page, ::ROWS, GUI::Scroll{0});
  GUI::set(page, ::ROWS, GUI::Cursor{0});
  VIEWS::SIGNALS::hang(::rows);
}

void took(GUI::Handle page) {
  const Whole row = GUI::GET::cursor(page, ::ROWS);
  if (row >= ::rows.size() || ::rows[row].words == 0) return;
  if (::node.empty()) return ::browse(page, ::rows[row].name);
  VIEWS::SIGNALS::aim({.node = ::node, .parameter = row});
  ::spread(page, false);
}

auto away() -> Flag {
  return VIEWS::elsewhere(::BUTTON, ::PANEL) &&
         VIEWS::elsewhere(::MEANT, ::PANEL);
}

}  // namespace

void SOUND::VIEWS::SIGNALS::aiming() {
  const GUI::Handle page = document();
  GUI::set(page, ::BUTTON, GUI::Visibility{true});
  if (GUI::GET::clicked(page, ::BUTTON) || GUI::GET::clicked(page, ::MEANT))
    return ::spread(page, !hung());
  if (!hung()) return;
  if (::rows.empty()) return ::spread(page, false);
  hang(::rows);
  if (GUI::GET::activated(page, ::ROWS)) return ::took(page);
  if (::away()) ::spread(page, false);
}

void SOUND::VIEWS::SIGNALS::aiming(SHELL::Session &session) {
  session.print(std::format("aimed {}", named()));
  if (!hung()) return;
  session.print(std::format(
    "aiming {} rows {} cursor {}", ::node.empty() ? "nodes" : ::node.c_str(),
    ::rows.size(), GUI::GET::cursor(document(), ::ROWS)));
  for (const Offer &offer : ::rows)
    session.print(std::format(
      "choice {} params {}", offer.name,
      offer.words == NONE ? String("none") : std::to_string(offer.words)));
}
