// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>
#include <utility>

#include "../../graph.hpp"
#include "graph.replace.hpp"

namespace {
using namespace SOUND;
namespace BROWSE = SOUND::VIEWS::WIRED::BROWSE;

constexpr STRING::Hot ADDS = "Add";

String pending;

auto played(const String &plugin) -> Flag {
  return BROWSE::shelved(GRAPH::takes(plugin), GRAPH::gives(plugin)) ==
         BROWSE::INSTRUMENTS;
}

auto sounding() -> String {
  const String root = VIEWS::WIRED::steering();
  String last;
  if (root.empty()) return last;
  for (const Node &node : GRAPH::held().nodes)
    if (
      node.seat == Node::PLUGIN && node.claim == root && ::played(node.plugin))
      last = node.name;
  return last;
}

void replace(const String &plugin) {
  VIEWS::WIRED::replace(::sounding(), plugin);
}

void add(const String &plugin) { VIEWS::WIRED::take(plugin); }

struct Act {
  STRING::Hot label = "";
  void (*act)(const String &plugin) = nullptr;
};

constexpr Act ACTS[] = {
  {.label = VIEWS::WIRED::REPLACES, .act = ::replace},
  {.label = ::ADDS, .act = ::add}};

}  // namespace

void SOUND::VIEWS::WIRED::take(
  const String &plugin, STRING::Hot list, Whole row) {
  if (!::played(plugin) || ::sounding().empty()) return take(plugin);
  const GUI::Handle page = document();
  const String cell =
    std::format("{}.{}", list, row - GUI::GET::first(page, list));
  const GUI::Position at = GUI::GET::origin(page, cell.c_str());
  const GUI::Extent size = GUI::GET::measured(page, cell.c_str());
  Vector<String> rows;
  for (const ::Act &act : ::ACTS) rows.push_back(String(act.label));
  ::pending = plugin;
  VIEWS::MENU::raise(rows, at.x + size.w, at.y + size.h, WHOSE);
  if (String(list) == BROWSE::ROWS) raise(among(plugin));
}

void SOUND::VIEWS::WIRED::asking() {
  if (::pending.empty()) return;
  const Whole row = VIEWS::MENU::taken(WHOSE);
  if (row == NONE && VIEWS::MENU::standing()) return;
  const String plugin = std::exchange(::pending, {});
  if (row < std::size(::ACTS)) ::ACTS[row].act(plugin);
}
