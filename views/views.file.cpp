// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/input.hpp>

#include "../commands.hpp"
#include "../session.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot DOOR = "file";
constexpr STRING::Hot ASK = "file.ask";
constexpr STRING::Hot NAME = "file.name";
constexpr STRING::Hot KEEP = "file.keep";
constexpr STRING::Hot FILED = "file";
constexpr STRING::Hot PICKS = "files";

const Vector<String> ROWS = {"New", "Open", "Save", "Save as..."};

enum Raised : Whole { DOWN, MAIN, PICK };
Whole raised = DOWN;
Vector<String> offered;

void menu(const Vector<String> &rows, STRING::Hot whose) {
  VIEWS::MENU::raise(rows, whose, ::DOOR);
}

void sheet(GUI::Handle page, Flag on) {
  GUI::set(page, ::ASK, GUI::Visibility{on});
  if (!on) return;
  const String name = SESSION::held().name;
  GUI::set(page, ::NAME, GUI::Text{name});
  GUI::edit(page, ::NAME);
  GUI::set(page, ::NAME, GUI::Caret{name.size(), 0});
}

void mained(GUI::Handle page, Whole row) {
  if (row == 0) return COMMANDS::fresh();
  if (row == 1) {
    ::offered = SESSION::names();
    if (::offered.empty()) return;
    ::menu(::offered, ::PICKS);
    ::raised = PICK;
    return;
  }
  if (row == 2 && !SESSION::held().name.empty())
    return void(COMMANDS::save(String(SESSION::held().name)));
  ::sheet(page, true);
}

void took(GUI::Handle page) {
  if (::raised == MAIN) {
    const Whole row = VIEWS::MENU::taken(::FILED);
    if (row == NONE) return;
    ::raised = DOWN;
    ::mained(page, row);
  } else if (::raised == PICK) {
    const Whole row = VIEWS::MENU::taken(::PICKS);
    if (row == NONE) return;
    ::raised = DOWN;
    if (row < ::offered.size()) COMMANDS::open(::offered[row]);
  }
}

void asked(GUI::Handle page) {
  if (!GUI::GET::visibility(page, ::ASK)) return;
  if (GUI::GET::committed(page, ::NAME) || GUI::GET::clicked(page, ::KEEP)) {
    const String name = GUI::GET::text(page, ::NAME);
    GUI::edit(page, "");
    if (name.empty()) return;
    COMMANDS::save(name);
    ::sheet(page, false);
    return;
  }
  if (INPUT::GET::pressed(INPUT::KEYS::ESCAPE)) ::sheet(page, false);
}

}  // namespace

void SOUND::VIEWS::file() {
  const GUI::Handle page = document();
  ::asked(page);
  ::took(page);
  if (::raised != DOWN && !VIEWS::MENU::standing()) ::raised = DOWN;
  if (!GUI::GET::clicked(page, ::DOOR)) return;
  if (::raised != DOWN) {
    VIEWS::MENU::lower();
    ::raised = DOWN;
    return;
  }
  ::sheet(page, false);
  ::menu(::ROWS, ::FILED);
  ::raised = MAIN;
}
