// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../boards.hpp"
#include "piano.paper.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot INK = "staffline";
constexpr Float WEST = 0.0f;

Whole stood = 0;
Whole born = 0;

auto north(GUI::Handle page, const String &plate) -> Float {
  return GUI::GET::position(page, plate.c_str()).y;
}

auto line(const String &plate, Whole at) -> String {
  return plate + "." + std::to_string(at);
}

}  // namespace

void SOUND::VIEWS::ROLL::STAFF::lined(
  GUI::Handle page, const String &plate, Float wide, Whole count) {
  if (VIEWS::reborn(::born)) ::stood = 0;
  for (Whole at = count; at < ::stood; ++at)
    BOARDS::drop(page, ::line(plate, at));
  for (Whole at = ::stood; at < count; ++at) {
    BOARDS::place(page, plate.c_str(), "panel", ::line(plate, at), {}, {});
    GUI::set(page, ::line(plate, at).c_str(), GUI::Style{::INK});
  }
  ::stood = count;
  for (Whole at = 0; at < count; ++at) {
    const String id = ::line(plate, at);
    const String drawn = rule(at);
    const Float y =
      GUI::GET::position(page, drawn.c_str()).y - ::north(page, plate);
    GUI::set(page, id.c_str(), GUI::Position{::WEST, y});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{wide, GUI::GET::extent(page, drawn.c_str()).h});
  }
}
