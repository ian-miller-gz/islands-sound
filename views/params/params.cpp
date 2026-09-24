// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/input.hpp>

#include "../../control.hpp"
#include "../../graph.hpp"
#include "params.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::RACK::Row;

constexpr STRING::Hot NAME = "params";
constexpr STRING::Hot ROWS = "params.rows";
constexpr STRING::Hot AIMED = "params.aimed";
constexpr STRING::Hot NAMED = "name";

auto aimed() -> String {
  const String node = CONTROL::aimed();
  return GRAPH::at(node) == NONE ? String() : node;
}

auto rows() -> Vector<Row> {
  const String aim = ::aimed();
  Vector<Row> wanted;
  for (const Node &node : GRAPH::held().nodes) {
    if (!aim.empty() && node.name != aim) continue;
    for (Whole at = 0; at < VIEWS::parameters(node.name); ++at)
      wanted.push_back({node.name, at});
  }
  return wanted;
}

auto named(const Row &row) -> String {
  return std::format(
    "{} {} {}", row.node, VIEWS::named(row.node),
    VIEWS::named(row.node, row.parameter));
}

auto stated(const Row &row) -> String {
  const String said =
    std::format("{} {}", ::named(row), VIEWS::RACK::shown(row));
  return VIEWS::RACK::listed(row) ? said + " chooser" : said;
}

auto whose() -> String {
  const String node = ::aimed();
  return node.empty() ? String() : VIEWS::named(node);
}

void run() {
  const GUI::Handle page = VIEWS::document();
  if (!VIEWS::RACK::choosing() && INPUT::GET::pressed(INPUT::KEYS::ESCAPE))
    CONTROL::aim({});
  GUI::set(page, ::AIMED, GUI::Text{::whose()});
  const Vector<Row> wanted = ::rows();
  GUI::set(page, ::ROWS, GUI::Rows{wanted.size()});
  const Whole first = GUI::GET::first(page, ::ROWS);
  for (Whole row = 0; first + row < wanted.size(); ++row) {
    const String cell = std::format("{}.{}", ::ROWS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    const Row &held = wanted[first + row];
    GUI::set(page, (cell + "." + ::NAMED).c_str(), GUI::Text{::named(held)});
    VIEWS::RACK::turn(page, cell, held);
    VIEWS::RACK::door(page, cell, held);
  }
  VIEWS::RACK::chooser();
}

void state(SHELL::Session &session) {
  const Vector<Row> wanted = ::rows();
  const CONTROL::Reel reel = CONTROL::reel();
  session.print(std::format(
    "params rows {} record {} take {} turned {}", wanted.size(),
    reel.armed ? "armed" : "off", reel.taking ? "running" : "idle",
    reel.turned));
  const String node = ::aimed();
  session.print(
    node.empty()
      ? String("aimed none")
      : std::format(
          "aimed {} {}", node, GUI::GET::text(VIEWS::document(), ::AIMED)));
  for (Whole row = 0; row < wanted.size(); ++row)
    session.print(std::format("row {} {}", row, ::stated(wanted[row])));
  VIEWS::RACK::chooser(session);
}

[[maybe_unused]] const Flag offered =
  VIEWS::offer({.name = ::NAME, .run = ::run, .state = ::state});

}  // namespace
