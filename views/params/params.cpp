// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/input.hpp>

#include "../../control.hpp"
#include "params.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::RACK::Row;

constexpr STRING::Hot NAME = "params";
constexpr STRING::Hot ROWS = "params.rows";
constexpr STRING::Hot AIMED = "params.aimed";

auto whose() -> String {
  const String node = VIEWS::RACK::aimed();
  return node.empty() ? String() : VIEWS::named(node);
}

void run() {
  const GUI::Handle page = VIEWS::document();
  VIEWS::RACK::cancel();
  if (!VIEWS::RACK::choosing() && INPUT::GET::pressed(INPUT::KEYS::ESCAPE))
    CONTROL::aim({});
  GUI::set(page, ::AIMED, GUI::Text{::whose()});
  const Vector<Row> wanted = VIEWS::RACK::rows();
  GUI::set(page, ::ROWS, GUI::Rows{wanted.size()});
  const Float column = VIEWS::RACK::column(page, wanted);
  const Whole first = GUI::GET::first(page, ::ROWS);
  for (Whole row = 0; first + row < wanted.size(); ++row) {
    const String cell = std::format("{}.{}", ::ROWS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    const Row &held = wanted[first + row];
    VIEWS::RACK::show(page, cell, held, column);
    VIEWS::RACK::turn(page, cell, held);
    VIEWS::RACK::door(page, cell, held);
  }
  VIEWS::RACK::chooser();
  VIEWS::RACK::menu(page);
}

void print(SHELL::Session &session, const Vector<Row> &wanted) {
  const GUI::Handle page = VIEWS::document();
  const Float column = VIEWS::RACK::column(page, wanted);
  for (Whole row = 0; row < wanted.size(); ++row) {
    const Row &held = wanted[row];
    session.print(std::format("row {} {}", row, VIEWS::RACK::stated(held)));
    if (VIEWS::RACK::head(held)) continue;
    const String whole = VIEWS::RACK::titled(held);
    if (VIEWS::RACK::cut(page, held, column) != whole)
      session.print(std::format("cut {} {}", row, whole));
  }
}

void state(SHELL::Session &session) {
  const Vector<Row> wanted = VIEWS::RACK::rows();
  const CONTROL::Reel reel = CONTROL::reel();
  session.print(std::format(
    "params rows {} record {} take {} turned {}", wanted.size(),
    reel.armed ? "armed" : "off", reel.taking ? "running" : "idle",
    reel.turned));
  const String node = VIEWS::RACK::aimed();
  session.print(
    node.empty()
      ? String("aimed none")
      : std::format(
          "aimed {} {}", node, GUI::GET::text(VIEWS::document(), ::AIMED)));
  ::print(session, wanted);
  VIEWS::RACK::bound(session);
  VIEWS::RACK::chooser(session);
}

[[maybe_unused]] const Flag offered = VIEWS::offer(
  {.name = ::NAME,
   .run = ::run,
   .state = ::state,
   .words = VIEWS::RACK::words(),
   .keep = VIEWS::RACK::keep,
   .restore = VIEWS::RACK::restore});

}  // namespace
