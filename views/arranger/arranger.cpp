// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../arrangement/arrangement.face.hpp"
#include "../lane/lane.face.hpp"
#include "../views.split.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::ARRANGER;

constexpr STRING::Hot TOOLS = "tools";
constexpr STRING::Hot MARKED = "selected";
constexpr STRING::Hot PLAIN = "icon";
constexpr STRING::Hot SESSION = "session";

Whole paging = NONE;

auto standers() -> Vector<const VIEWS::View *> {
  return {&VIEWS::ARRANGEMENT::face(), &VIEWS::LANE::face()};
}

auto stander() -> const VIEWS::View & {
  return ::paging == NONE ? VIEWS::ARRANGEMENT::face() : VIEWS::LANE::face();
}

void band(GUI::Handle page) {
  const String id = String(NAME) + "." + ::SESSION;
  if (GUI::GET::clicked(page, id.c_str())) VIEWS::ARRANGER::turned(NONE);
  GUI::set(page, id.c_str(), GUI::Style{::paging == NONE ? ::MARKED : ::PLAIN});
}

void gated(GUI::Handle page) {
  const VIEWS::View &stands = ::stander();
  for (const VIEWS::View *unit : ::standers()) {
    const Flag on = unit == &stands;
    VIEWS::show(*unit, on);
    GUI::set(
      page, (String(unit->name) + "." + ::TOOLS).c_str(), GUI::Visibility{on});
  }
}

void run() {
  const GUI::Handle page = VIEWS::document();
  ::band(page);
  VIEWS::ARRANGER::label(page);
  if (VIEWS::ARRANGER::below(page)) return;
  ::gated(page);
  if (::paging != NONE) VIEWS::LANE::paged(::paging);
  ::stander().run();
}

void state(SHELL::Session &session) {
  const VIEWS::View &stands = ::stander();
  session.print(std::format(
    "arranger page {} face {}", VIEWS::ARRANGER::spoken(), stands.name));
  if (VIEWS::SPLIT::shared()) VIEWS::ARRANGER::below(session);
  VIEWS::LANE::marked(session);
  VIEWS::LANE::faces(session);
  if (stands.state != nullptr) stands.state(session);
}

[[maybe_unused]] const Flag offered = VIEWS::offer(
  {.name = NAME,
   .opening = true,
   .controls = {VIEWS::ZOOM, VIEWS::PAN},
   .run = ::run,
   .door = VIEWS::ARRANGER::door,
   .state = ::state,
   .words = VIEWS::ARRANGER::words(),
   .faced = VIEWS::ARRANGER::faced,
   .keep = VIEWS::ARRANGER::keep,
   .restore = VIEWS::ARRANGER::restore,
   .shed = VIEWS::ARRANGER::shed});
[[maybe_unused]] const Flag hosted = VIEWS::SPLIT::host(NAME);
[[maybe_unused]] const Flag owned = VIEWS::ARRANGER::own();

}  // namespace

auto SOUND::VIEWS::ARRANGER::turned() -> Whole { return ::paging; }

auto SOUND::VIEWS::ARRANGER::turned(Whole kind) -> Flag {
  if (kind != NONE && !LANE::editable(kind)) return false;
  ::paging = kind;
  return true;
}

auto SOUND::VIEWS::ARRANGER::spoken() -> STRING::Hot {
  return ::paging == NONE ? ::SESSION : KIND::spoken(::paging);
}

auto SOUND::VIEWS::ARRANGER::faced() -> const View * {
  return SPLIT::shared() ? focused() : &::stander();
}
