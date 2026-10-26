// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>
#include <island/gui/window.hpp>

#include "../../boards.hpp"
#include "../../graph.hpp"
#include "../../timeline.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LIST = "tracks";
constexpr Float UNMEASURED = 0.0f;

String followed;

void panned(GUI::Handle page, const String &root) {
  const String board = VIEWS::WIRED::board();
  const String box = GUI::SAC::SEAT::named(board.c_str(), GRAPH::at(root));
  const GUI::Extent seen = VIEWS::WIRED::window();
  const GUI::Position at = GUI::GET::position(page, box.c_str());
  const GUI::Extent size = GUI::GET::extent(page, box.c_str());
  const GUI::NGA::Pan held = GUI::NGA::GET::pan(page, board.c_str());
  GUI::NGA::set(
    page, board.c_str(),
    GUI::NGA::Pan{
      GUI::SAC::WINDOW::centre(held.x, at.x, size.w, seen.w),
      GUI::SAC::WINDOW::centre(held.y, at.y, size.h, seen.h)});
}

}  // namespace

auto SOUND::VIEWS::WIRED::attended() -> String {
  return browsing() ? String() : steering();
}

auto SOUND::VIEWS::WIRED::steering() -> String {
  const Whole track = steered();
  if (track == NONE) return {};
  const String root = TIMELINE::held().tracks[track].root;
  return GRAPH::rooted(root) ? root : String();
}

auto SOUND::VIEWS::WIRED::steered() -> Whole {
  const Whole track = VIEWS::cursor(::LIST);
  return track < TIMELINE::held().tracks.size() ? track : NONE;
}

void SOUND::VIEWS::WIRED::follow(Flag again) {
  const GUI::Handle page = document();
  const GUI::Extent view = GUI::GET::measured(page, board().c_str());
  if (view.w <= ::UNMEASURED || view.h <= ::UNMEASURED) return;
  const String root = attended();
  if (!again && root == ::followed) return;
  ::followed = root;
  if (!root.empty()) ::panned(page, root);
}
