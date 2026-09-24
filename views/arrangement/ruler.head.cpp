// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/playhead.hpp>

#include "../../transport.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot HEAD = "head";
constexpr STRING::Hot INK = "rulerhead";
constexpr Float CAP = 6.0f;
constexpr Float NORTH = 0.0f;
constexpr Float OVER = 1.0f;

Flag stood = false;
Whole born = 0;

auto capped() -> String { return VIEWS::ARRANGEMENT::strip() + "." + ::HEAD; }

auto depth(GUI::Handle page) -> Float {
  return GUI::GET::measured(page, VIEWS::ARRANGEMENT::strip().c_str()).h;
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::cap() {
  const GUI::Handle page = document();
  const String id = ::capped();
  if (VIEWS::reborn(::born)) ::stood = false;
  if (!::stood) {
    GUI::NODES::create(page, strip().c_str(), "panel", id.c_str());
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    GUI::set(page, id.c_str(), GUI::Depth{::OVER});
    ::stood = true;
  }
  const Float scale = GUI::NGA::GET::zoom(page, board().c_str()).value;
  const Float west = GUI::NGA::GET::pan(page, board().c_str()).x;
  const GUI::SAC::PLAYHEAD::Mark mark =
    GUI::SAC::PLAYHEAD::stripcap(across(VIEWS::clock()), west, scale, ::CAP);
  GUI::set(page, id.c_str(), GUI::Position{mark.at, ::NORTH});
  GUI::set(page, id.c_str(), GUI::Extent{mark.wide, ::depth(page)});
}
