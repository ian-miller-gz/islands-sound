// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../arrangement/arrangement.face.hpp"
#include "../lane/lane.face.hpp"
#include "../views.split.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::ARRANGER;
using VIEWS::SPLIT::UPPER;

constexpr STRING::Hot TOOLS = "tools";
constexpr STRING::Hot MARKED = "selected";
constexpr STRING::Hot PLAIN = "icon";
constexpr STRING::Hot SESSION = "session";

auto page() -> Whole { return VIEWS::ARRANGER::turned(); }

auto standers() -> Vector<const VIEWS::View *> {
  return {&VIEWS::ARRANGEMENT::face(), &VIEWS::LANE::face()};
}

auto stander() -> const VIEWS::View & {
  return ::page() == NONE ? VIEWS::ARRANGEMENT::face() : VIEWS::LANE::face();
}

void band(GUI::Handle page) {
  const String id = String(NAME) + "." + ::SESSION;
  if (GUI::GET::clicked(page, id.c_str())) VIEWS::ARRANGER::turned(NONE);
  GUI::set(page, id.c_str(), GUI::Style{::page() == NONE ? ::MARKED : ::PLAIN});
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
  if (!VIEWS::ARRANGER::seen())
    VIEWS::ARRANGER::turned(
      VIEWS::ARRANGER::paged(UPPER) == NONE ? VIEWS::ARRANGER::offered().kind
                                             : NONE);
  ::band(page);
  VIEWS::ARRANGER::label(page);
  ::gated(page);
  VIEWS::ARRANGER::TOOLS::seat(page);
  if (::page() != NONE) VIEWS::LANE::paged(::page());
  ::stander().run();
}

void state(SHELL::Session &session) {
  const VIEWS::View &stands = ::stander();
  session.print(std::format(
    "arranger page {} face {}", VIEWS::ARRANGER::spoken(), stands.name));
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
[[maybe_unused]] const Flag owned = VIEWS::ARRANGER::own();

}  // namespace

auto SOUND::VIEWS::ARRANGER::faced() -> const View * { return &::stander(); }
