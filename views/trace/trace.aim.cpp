// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../control.hpp"
#include "trace.internal.hpp"

namespace {
using namespace SOUND;
namespace TRACE = SOUND::VIEWS::TRACE;

String mirrored;

auto taken(GUI::Handle page) -> Flag {
  const Flag selected = GUI::GET::selected(page, TRACE::ROWS);
  return GUI::GET::activated(page, TRACE::ROWS) || selected;
}

auto row(const Vector<GRAPH::TRACE::Step> &steps, const String &node) -> Whole {
  for (Whole at = 0; at < steps.size(); ++at)
    if (steps[at].node == node) return at;
  return NONE;
}

}  // namespace

void SOUND::VIEWS::TRACE::aimed(GUI::Handle page) {
  const Vector<GRAPH::TRACE::Step> steps = walked();
  const Whole cursor = GUI::GET::cursor(page, ROWS);
  if (::taken(page) && cursor < steps.size()) {
    CONTROL::aim(steps[cursor].node);
    ::mirrored = steps[cursor].node;
  }
  const String aim = CONTROL::aimed();
  if (aim == ::mirrored) return;
  ::mirrored = aim;
  const Whole at = ::row(steps, aim);
  if (at != NONE) GUI::set(page, ROWS, GUI::Cursor{at});
}

void SOUND::VIEWS::TRACE::aimed(SHELL::Session &session) {
  session.print(std::format("cursor {}", GUI::GET::cursor(document(), ROWS)));
}
