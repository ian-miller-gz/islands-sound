// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>
#include <island/gui/gui.hpp>

#include "../kind.hpp"
#include "views.keys.hpp"

namespace SOUND::VIEWS::HOTKEYS::FAMILY {

constexpr STRING::Hot ROLL = "roll";
constexpr STRING::Hot ARRANGEMENT = "arrangement";
constexpr STRING::Hot GRAPH = "graph";
constexpr STRING::Hot SPLIT = "split";
constexpr STRING::Hot SESSION = "session";
constexpr STRING::Hot ORDER[] = {ROLL, ARRANGEMENT, GRAPH, SPLIT, SESSION};
static_assert(SOUND::KIND::named(ORDER));

}  // namespace SOUND::VIEWS::HOTKEYS::FAMILY

namespace SOUND::VIEWS::HOTKEYS {

struct Row {
  STRING::Hot family = "", act = "";
  Chord chord;
};

auto listed() -> Vector<Row>;

auto spelled(const Chord &chord) -> String;

auto take(const Row &row) -> Flag;

template <typename Entry>
auto keyed(const Entry &entry) -> Chord {
  if constexpr (requires { entry.chord; }) {
    return entry.chord;
  } else {
    Chord chord{
      .action = entry.action, .code = entry.letter, .control = entry.control};
    if constexpr (requires { entry.shift; }) chord.shift = entry.shift;
    return chord;
  }
}

template <typename Table>
auto offer(STRING::Hot family, const Table &table) -> Flag {
  for (const auto &entry : table)
    if (entry.name[0] != '\0') take({family, entry.name, keyed(entry)});
  return true;
}

void paged(GUI::Handle page, Flag keys);

void turned(GUI::Handle page);

}  // namespace SOUND::VIEWS::HOTKEYS
