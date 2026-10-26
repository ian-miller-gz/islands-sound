// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include <island/gui/nga.hpp>

#include "../../timeline.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;
namespace LANE = SOUND::VIEWS::LANE;

constexpr STRING::Hot DOOR = "return";
constexpr Float ORIGIN = 0.0f;

struct Seen {
  const LANE::Face *face = nullptr;
  Float zoom = 0.0f;
};

Seen seen;

auto door() -> String { return String(LANE::face().name) + "." + ::DOOR; }

auto rail(const GUI::NGA::Bounds &walls, Float zoom) -> Float {
  if (walls.west == GUI::Walls::NONE) return ::ORIGIN;
  return std::max(::ORIGIN, -walls.west * zoom);
}

void floored(
  GUI::Handle page, const String &id, const LANE::Face &one, Float floor) {
  const GUI::NGA::Zoom zoom = GUI::NGA::GET::zoom(page, id.c_str());
  const Flag moved = ::seen.face == &one && zoom.value != ::seen.zoom;
  if (moved && zoom.value < floor) {
    const VIEWS::View *face = LANE::faced();
    const Float down =
      face->split || face->flat ? zoom.down : GUI::NGA::Zoom::SAME;
    GUI::NGA::set(page, id.c_str(), GUI::NGA::Zoom{floor, down});
  }
  ::seen = {&one, GUI::NGA::GET::zoom(page, id.c_str()).value};
}

void walled(
  GUI::Handle page, const LANE::Face &one, const LANE::Clip &clip,
  const VIEWS::Overview &read) {
  const String id = LANE::field();
  const GUI::NGA::Bounds walls = GUI::NGA::GET::bounds(page, id.c_str());
  const Float rail = ::rail(walls, GUI::NGA::GET::zoom(page, id.c_str()).value);
  const Float opens = read.placed(clip.at);
  const Float closes = read.placed(clip.at + clip.frames);
  const Float wide = GUI::GET::measured(page, id.c_str()).w;
  if (closes > opens && wide > rail)
    ::floored(page, id, one, (wide - rail) / (closes - opens));
  const Float zoom = GUI::NGA::GET::zoom(page, id.c_str()).value;
  GUI::NGA::set(
    page, id.c_str(),
    GUI::NGA::Bounds{opens - rail / zoom, closes, walls.north, walls.south});
  LANE::dim(clip, read, rail);
}

}  // namespace

void SOUND::VIEWS::LANE::bound() {
  const GUI::Handle page = document();
  const String door = ::door();
  if (GUI::GET::clicked(page, door.c_str())) scope(NONE);
  Face *one = held();
  if (one != nullptr && one->clip != NONE && GET::scoped() == nullptr)
    scope(NONE);
  const Clip *clip = GET::scoped();
  GUI::set(page, door.c_str(), GUI::Visibility{clip != nullptr});
  const Overview read = clip == nullptr ? Overview{} : faced()->overview();
  if (clip == nullptr || read.placed == nullptr) {
    ::seen = {};
    return dim();
  }
  ::walled(page, *one, *clip, read);
}
