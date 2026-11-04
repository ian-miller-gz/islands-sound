// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "views.hotkeys.hpp"
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::SHUT;
namespace KEYS = INPUT::KEYS;

constexpr Float STEP = 8.0f, LARGE = 80.0f;
constexpr STRING::Hot FIRST = "split.arranger";

Float from = SHUT;

struct Held {
  VIEWS::Chord chord;
  auto (*act)(Float by) -> Flag = nullptr;
  Float by = SHUT;
  STRING::Hot name = "";
};

auto least(const VIEWS::SPLIT::Seat &seat) -> Float {
  return seat.bar + seat.least + seat.least;
}

auto moved(Float by) -> Flag {
  const VIEWS::SPLIT::Seat seat = VIEWS::SPLIT::measured(VIEWS::sheet());
  const Float now = std::min(VIEWS::SPLIT::height(), seat.room);
  VIEWS::SPLIT::seat(std::max(now + by, ::least(seat)));
  return true;
}

auto most(Float) -> Flag {
  VIEWS::SPLIT::seat(VIEWS::SPLIT::measured(VIEWS::sheet()).room);
  return true;
}

auto floored(Float) -> Flag {
  VIEWS::SPLIT::seat(::least(VIEWS::SPLIT::measured(VIEWS::sheet())));
  return true;
}

auto toggled(Float) -> Flag {
  VIEWS::SPLIT::toggle();
  return true;
}

auto back(Float) -> Flag {
  VIEWS::SPLIT::seat(::from);
  return false;
}

auto passed(Float) -> Flag {
  GUI::edit(VIEWS::sheet(), ::FIRST, false);
  return false;
}

constexpr Held HELD[] = {
  {{.action = KEYS::UP}, ::moved, ::STEP, "grow"},
  {{.action = KEYS::UP, .shift = true}, ::moved, ::LARGE, "grow"},
  {{.action = KEYS::DOWN}, ::moved, -::STEP, "shrink"},
  {{.action = KEYS::DOWN, .shift = true}, ::moved, -::LARGE, "shrink"},
  {{.action = KEYS::HOME}, ::most, SHUT, "fill"},
  {{.action = KEYS::END}, ::floored, SHUT, "fold"},
  {{.action = KEYS::ENTER}, ::toggled, SHUT, "split"},
  {{.code = KEYS::ESCAPE}, ::back, SHUT, "cancel"},
  {{.code = KEYS::TAB}, ::passed, SHUT, "leave"}};

[[maybe_unused]] const Flag listed =
  VIEWS::HOTKEYS::offer(VIEWS::HOTKEYS::FAMILY::SPLIT, ::HELD);

auto own(const String &id) -> Flag {
  return id == VIEWS::SPLIT::BAR || id == VIEWS::SPLIT::HANDLE;
}

auto elsewhere() -> Flag {
  const GUI::Handle page = VIEWS::sheet();
  if (GUI::GET::pressed(page) && !::own(GUI::GET::hover(page))) return true;
  const GUI::Handle twin = VIEWS::SPLIT::second();
  return VIEWS::SPLIT::doubled() && GUI::GET::pressed(twin);
}

}  // namespace

void SOUND::VIEWS::SPLIT::HOLD::take() {
  ::from = height();
  GUI::edit(sheet(), "");
}

auto SOUND::VIEWS::SPLIT::HOLD::keyed() -> Flag {
  if (::elsewhere()) return false;
  for (const ::Held &row : ::HELD)
    if (chorded(row.chord)) return row.act(row.by);
  return true;
}
