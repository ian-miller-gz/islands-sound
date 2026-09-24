// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../arrangement/arrangement.face.hpp"
#include "../lane/lane.face.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::ARRANGER;

constexpr STRING::Hot TOOLS = "tools";
constexpr STRING::Hot MARKED = "selected";
constexpr STRING::Hot PLAIN = "icon";
const String PAGED = String(NAME) + ".page";

Whole paging = 0;

auto worded(const Page &one) -> String { return String(NAME) + "." + one.word; }

auto standers() -> Vector<const VIEWS::View *> {
  return {&VIEWS::ARRANGEMENT::face(), &VIEWS::LANE::face()};
}

auto stander() -> const VIEWS::View & {
  return PAGES[::paging].kind == NONE ? VIEWS::ARRANGEMENT::face()
                                      : VIEWS::LANE::face();
}

void band(GUI::Handle page) {
  for (const Page &one : PAGES)
    if (GUI::GET::clicked(page, ::worded(one).c_str()))
      ::paging = static_cast<Whole>(&one - PAGES);
  for (const Page &one : PAGES)
    GUI::set(
      page, ::worded(one).c_str(),
      GUI::Style{&one == &PAGES[::paging] ? ::MARKED : ::PLAIN});
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
  ::gated(page);
  if (PAGES[::paging].kind != NONE) VIEWS::LANE::paged(PAGES[::paging].kind);
  ::stander().run();
}

void state(SHELL::Session &session) {
  const VIEWS::View &stands = ::stander();
  session.print(
    std::format("arranger page {} face {}", PAGES[::paging].word, stands.name));
  if (stands.state != nullptr) stands.state(session);
}

void keep(FIELDS::Map &face) { face[::PAGED] = String(PAGES[::paging].word); }

void restore(const FIELDS::Map &face) {
  const auto found = face.find(::PAGED);
  if (found == face.end()) return;
  const String *word = std::get_if<String>(&found->second);
  if (word == nullptr) return;
  for (const Page &one : PAGES)
    if (*word == one.word) ::paging = static_cast<Whole>(&one - PAGES);
}

[[maybe_unused]] const Flag offered = VIEWS::offer(
  {.name = NAME,
   .opening = true,
   .controls = {VIEWS::ZOOM, VIEWS::PAN},
   .run = ::run,
   .door = VIEWS::ARRANGER::door,
   .state = ::state,
   .faced = VIEWS::ARRANGER::faced,
   .keep = ::keep,
   .restore = ::restore,
   .shed = VIEWS::ARRANGER::shed});

}  // namespace

auto SOUND::VIEWS::ARRANGER::turned() -> Whole { return ::paging; }

auto SOUND::VIEWS::ARRANGER::turned(Whole page) -> Flag {
  if (page >= COUNT) return false;
  ::paging = page;
  return true;
}

auto SOUND::VIEWS::ARRANGER::faced() -> const View * { return &::stander(); }
