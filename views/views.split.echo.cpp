// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../graph.hpp"
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;

struct Wanted {
  GUI::Position at;
  GUI::Extent size;
  Flag on = false;
};
Wanted wanted;

auto source(GUI::Handle page) -> ::Wanted {
  if (::wanted.on) return ::wanted;
  if (!VIEWS::SPLIT::doubled()) return {};
  const STRING::Hot root = VIEWS::roster()[VIEWS::chosen()].name;
  return {GUI::GET::origin(page, root), GUI::GET::measured(page, root), true};
}

}  // namespace

auto SOUND::VIEWS::SPLIT::doubled() -> Flag {
  const Whole at = lower();
  return at != NONE && at == chosen() && mirrored(at);
}

void SOUND::VIEWS::SPLIT::echo(GUI::Position at, GUI::Extent size) {
  ::wanted = {at, size, true};
}

/// @brief The lower region as a second viewport onto the upper's view: the
/// one sheet's region holding that view is echoed down into the lower
/// ground's rect (GUI::echo — the frame's draws shown again, a pointer in
/// the echo folding back onto the view), so nothing is laid or cached
/// twice. The echo is the chosen view's root when the lower region holds
/// it, or the rect a view asked for this frame (a face the arranger holds
/// in both regions), and none otherwise.
void SOUND::VIEWS::SPLIT::draw(GFX::Pass &) {
  const GUI::Handle page = sheet();
  const ::Wanted from = ::source(page);
  ::wanted = {};
  GUI::Echo echo;
  if (open() && from.on && from.size.w > 0.0f && from.size.h > 0.0f) {
    const GUI::Position ground = GUI::GET::origin(page, GROUND::LOWER);
    const GUI::Extent room = GUI::GET::measured(page, GROUND::LOWER);
    const Float down = GUI::GET::origin(page, MAIN).y + top();
    const Float tall = ground.y + room.h - down;
    echo = {
      from.at.x, from.at.y, from.size.w, std::min(from.size.h, tall), 0.0f,
      down - from.at.y};
  }
  GUI::echo(page, echo);
}
