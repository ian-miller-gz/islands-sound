// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>
#include <island/gui/playhead.hpp>

#include "../../boards.hpp"
#include "../../transport.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot HEAD = "head";
constexpr STRING::Hot MARKED = "selected";
constexpr Float HAIR = 2.0f;
constexpr Float NORTH = 0.0f;

Flag stood = false;
Whole born = 0;

auto blade() -> String { return VIEWS::ARRANGEMENT::board() + "." + ::HEAD; }

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::head() {
  const GUI::Handle page = document();
  const String id = ::blade();
  if (VIEWS::reborn(::born)) ::stood = false;
  if (!::stood) {
    BOARDS::place(page, board().c_str(), "panel", id, {}, {::HAIR, MARGIN});
    GUI::set(page, id.c_str(), GUI::Style{::MARKED});
    GUI::set(page, id.c_str(), GUI::Depth{BLADE});
    ::stood = true;
  }
  const Float scale = GUI::NGA::GET::zoom(page, board().c_str()).value;
  const GUI::SAC::PLAYHEAD::Mark mark =
    GUI::SAC::PLAYHEAD::blade(across(VIEWS::clock()), scale, ::HAIR);
  GUI::set(page, id.c_str(), GUI::Position{mark.at, ::NORTH});
  GUI::set(page, id.c_str(), GUI::Extent{mark.wide, deep()});
}
