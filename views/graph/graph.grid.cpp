// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/ladder.hpp>
#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "graph.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot STEM = "grid";
constexpr STRING::Hot FINER = "fine";
constexpr STRING::Hot COARSER = "coarse";
constexpr STRING::Hot ACROSS = "across";
constexpr STRING::Hot DOWN = "down";
constexpr Flag WALKED = true;
constexpr Flag SIDLED = false;
constexpr Float HAIR = 2.0f;
constexpr Whole GRAIN = 100;
constexpr Float CROWD = 240.0f;
constexpr Whole THINNER = 2;
constexpr Float NOWHERE = 0.0f;

Whole stood = 0;
Whole born = 0;

auto mark(STRING::Hot way, Whole at) -> String {
  return std::format("{}.{}.{}.{}", VIEWS::WIRED::board(), ::STEM, way, at);
}

auto dressed(Flag opens) -> String {
  return String(::STEM) + (opens ? ::COARSER : ::FINER);
}

void lay(
  Flag walked, const Vector<GUI::SAC::LADDER::Mark> &marks, Float cross,
  GUI::Extent size) {
  const GUI::Handle page = VIEWS::document();
  for (Whole tick = 0; tick < ::stood; ++tick) {
    const String id = ::mark(walked ? ::DOWN : ::ACROSS, tick);
    const Float place = Float(marks[tick].at);
    GUI::set(
      page, id.c_str(),
      GUI::Position{walked ? cross : place, walked ? place : cross});
    GUI::set(page, id.c_str(), size);
    GUI::set(
      page, id.c_str(), GUI::Style{::dressed(marks[tick].opens).c_str()});
  }
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  const String board = VIEWS::WIRED::board();
  for (Whole at = count; at < ::stood; ++at) {
    GUI::NODES::remove(page, ::mark(::ACROSS, at).c_str());
    GUI::NODES::remove(page, ::mark(::DOWN, at).c_str());
  }
  for (Whole at = ::stood; at < count; ++at) {
    BOARDS::place(page, board.c_str(), "panel", ::mark(::ACROSS, at), {}, {});
    BOARDS::place(page, board.c_str(), "panel", ::mark(::DOWN, at), {}, {});
  }
  ::stood = count;
}

}  // namespace

void SOUND::VIEWS::WIRED::grid() {
  const GUI::Handle page = VIEWS::document();
  const String board = WIRED::board();
  const GUI::Extent view = WIRED::window();
  const Float scale = GUI::NGA::GET::zoom(page, board.c_str()).value;
  if (VIEWS::reborn(::born)) ::stood = 0;
  if (view.w <= ::NOWHERE || view.h <= ::NOWHERE) {
    ::build(0);
    return;
  }
  const Float reach = std::max(view.w, view.h);
  const Whole wide =
    GUI::SAC::LADDER::climb(::GRAIN, ::THINNER, scale, ::CROWD, reach);
  const GUI::NGA::Pan pan = GUI::NGA::GET::pan(page, board.c_str());
  const Vector<GUI::SAC::LADDER::Mark> sidled =
    GUI::SAC::LADDER::dress({wide}, wide * ::THINNER, pan.x, reach);
  const Vector<GUI::SAC::LADDER::Mark> walked =
    GUI::SAC::LADDER::dress({wide}, wide * ::THINNER, pan.y, reach);
  if (sidled.size() != ::stood) ::build(sidled.size());
  ::lay(::SIDLED, sidled, pan.y, {::HAIR, view.h});
  ::lay(::WALKED, walked, pan.x, {view.w, ::HAIR});
}
