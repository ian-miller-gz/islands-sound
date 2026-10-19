// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../arrangement/arrangement.face.hpp"
#include "../lane/lane.face.hpp"
#include "../views.split.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot TOOLS = "tools";
constexpr STRING::Hot FOCUS = "focus";
constexpr Float WHOLE = 0.0f;

Flag laid = false;

auto faces() -> Vector<const VIEWS::View *> {
  return {&VIEWS::ARRANGEMENT::face(), &VIEWS::LANE::face()};
}

void placed(GUI::Handle page, STRING::Hot root, Float down, Float margin) {
  const GUI::Position at = GUI::GET::position(page, root);
  const GUI::Extent size = GUI::GET::extent(page, root);
  if (at.y != down) GUI::set(page, root, GUI::Position{at.x, down});
  if (size.h != margin) GUI::set(page, root, GUI::Extent{size.w, margin});
}

void joined(GUI::Handle page) {
  if (!::laid) return;
  for (const VIEWS::View *unit : ::faces())
    ::placed(page, unit->name, ::WHOLE, ::WHOLE);
  ::laid = false;
}

void shown(GUI::Handle page, const VIEWS::View *focus) {
  for (const VIEWS::View *unit : ::faces()) {
    VIEWS::show(*unit, true);
    GUI::set(
      page, (String(unit->name) + "." + ::TOOLS).c_str(),
      GUI::Visibility{unit == focus});
  }
}

}  // namespace

auto SOUND::VIEWS::ARRANGER::below(GUI::Handle page) -> Flag {
  if (!SPLIT::shared()) {
    ::joined(page);
    return false;
  }
  if (!::laid) focus(LANE::face());
  turned(SESSION);
  const LANE::Mark mark = offered();
  if (mark.kind != NONE) LANE::paged(mark.kind);
  ::shown(page, focused());
  ::placed(page, ARRANGEMENT::face().name, ::WHOLE, SPLIT::height());
  ::placed(page, LANE::face().name, SPLIT::top(), ::WHOLE);
  for (const View *unit : ::faces()) unit->run();
  ::laid = true;
  return true;
}

void SOUND::VIEWS::ARRANGER::below(SHELL::Session &session) {
  const LANE::Mark mark = offered();
  session.print(std::format(
    "{} {} {} {} {}", NAME, SPLIT::LOWER, KIND::spoken(mark.kind), ::FOCUS,
    focused()->name));
  const View &lane = LANE::face();
  if (lane.state != nullptr) lane.state(session);
}
