// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "../../kind.hpp"
#include "../signals/signals.face.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;
namespace LANE = SOUND::VIEWS::LANE;

struct Held {
  String track, lane;
  Whole kind = NONE;
  auto operator==(const Held &) const -> bool = default;
};

struct Stood {
  GUI::Handle page = GUI::NONE;
  Held held;
};

Held held;
Vector<Stood> stood;
Whole born = 0;
/// @brief The held face's first showing on this page: written to the page's
/// own cells before it is read back from them (the split's second page).
auto arrived(GUI::Handle page) -> Flag {
  for (Stood &one : ::stood)
    if (one.page == page) {
      if (one.held == ::held) return false;
      one.held = ::held;
      return true;
    }
  ::stood.push_back({page, ::held});
  return true;
}

auto found() -> LANE::Face * {
  for (LANE::Face &one : LANE::records())
    if (one.track == ::held.track && one.lane == ::held.lane) return &one;
  return nullptr;
}

void read(LANE::Face &one, Whole kind) {
  const GUI::Handle page = VIEWS::document();
  const String id = LANE::field(kind);
  one.pan = GUI::NGA::GET::pan(page, id.c_str());
  one.zoom = GUI::NGA::GET::zoom(page, id.c_str());
  if (kind != KIND::NOTES) one.number = VIEWS::SIGNALS::number();
}

void write(const LANE::Face &one, Whole kind) {
  if (one.zoom.value <= LANE::UNSEEN) return;
  const GUI::Handle page = VIEWS::document();
  const String id = LANE::field(kind);
  GUI::NGA::set(page, id.c_str(), one.zoom);
  GUI::NGA::set(page, id.c_str(), one.pan);
  if (kind == KIND::CONTROL && one.number != NONE)
    VIEWS::SIGNALS::number(one.number);
}

void cleared(Whole kind) {
  if (kind == NONE) return;
  const GUI::Handle page = VIEWS::document();
  const String id = LANE::field(kind);
  for (const STRING::Cold &chosen : GUI::NGA::GET::selections(page, id.c_str()))
    GUI::NGA::set(page, chosen.c_str(), GUI::NGA::Selected{false});
}

}  // namespace

void SOUND::VIEWS::LANE::record() {
  const Flag fresh = VIEWS::reborn(::born);
  Face *one = ::found();
  if (one == nullptr) return;
  if (fresh || ::arrived(VIEWS::document())) return ::write(*one, ::held.kind);
  ::read(*one, ::held.kind);
}

void SOUND::VIEWS::LANE::forget() { ::held = {}; }

auto SOUND::VIEWS::LANE::held() -> Face * { return ::found(); }

auto SOUND::VIEWS::LANE::GET::seen() -> Flag {
  const Face *one = ::found();
  return one != nullptr && one->zoom.value > UNSEEN;
}

void SOUND::VIEWS::LANE::swap(Whole kind) {
  record();
  const Mark mark = edited(kind);
  if (mark.lane == NONE) return;
  const Face &one = faced(mark);
  if (one.track == ::held.track && one.lane == ::held.lane) return;
  ::cleared(::held.kind);
  ::held = {one.track, one.lane, mark.kind};
  ::write(one, mark.kind);
  ::cleared(mark.kind);
}
