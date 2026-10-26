// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "params.internal.hpp"
#include "params.kept.hpp"
#include "params.map.hpp"
#include "params.overruled.hpp"

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
  const Flag spent = VIEWS::RACK::cancel(page);
  if (!spent && !VIEWS::RACK::choosing() && VIEWS::escaped()) CONTROL::aim({});
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
  VIEWS::RACK::mapped(page);
  VIEWS::journaled(VIEWS::struck());
  VIEWS::RACK::settle();
}

void crop(SHELL::Session &session, Whole row, const Row &held) {
  const GUI::Handle page = VIEWS::document();
  const Whole first = GUI::GET::first(page, ::ROWS);
  if (row < first) return;
  const String cell = std::format("{}.{}", ::ROWS, row - first);
  if (!GUI::GET::visibility(page, cell.c_str())) return;
  const String whole = VIEWS::RACK::shown(held);
  if (VIEWS::RACK::fitted(page, cell, held) != whole)
    session.print(std::format("cropped {} {}", row, whole));
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
    ::crop(session, row, held);
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
  VIEWS::RACK::overruled(session);
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
