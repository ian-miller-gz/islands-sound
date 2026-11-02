// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "views.changes.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;
namespace CHANGES = VIEWS::CHANGES;

constexpr STRING::Hot GRIP = "grip";
constexpr STRING::Hot DRESS = "handle";
constexpr Float OVER = 2.0f;
constexpr Float AIR = 1.0f;

struct Held {
  String strip;
  Vector<String> grips;
  Whole born = 0;
};

VIEWS::Sided<Vector<Held>> held;
String carrying;

auto grip(const CHANGES::Mark &mark) -> String {
  return mark.id + "." + ::GRIP;
}

auto standing(const String &strip) -> Held & {
  for (Held &one : ::held())
    if (one.strip == strip) return one;
  ::held().push_back(Held{strip});
  return ::held().back();
}

void create(GUI::Handle page, const String &strip, const String &id) {
  GUI::NODES::create(page, strip.c_str(), ::GRIP, id.c_str());
  GUI::set(page, id.c_str(), GUI::Style{::DRESS});
  GUI::set(page, id.c_str(), GUI::Depth{::OVER});
  GUI::set(page, id.c_str(), GUI::Axis{.across = true});
  GUI::set(
    page, id.c_str(), GUI::Extent{CHANGES::HANDLE, VIEWS::BAND + CHANGES::INSET});
}

void rebuilt(
  GUI::Handle page, const String &strip, Held &held,
  const Vector<CHANGES::Mark> &marks) {
  if (VIEWS::reborn(held.born)) held.grips.clear();
  Vector<String> wanted;
  for (const CHANGES::Mark &mark : marks) wanted.push_back(::grip(mark));
  for (const String &id : held.grips)
    if (std::ranges::find(wanted, id) == wanted.end())
      GUI::NODES::remove(page, id.c_str());
  for (const String &id : wanted)
    if (std::ranges::find(held.grips, id) == held.grips.end())
      ::create(page, strip, id);
  held.grips = wanted;
}

void pressing(GUI::Handle page, const Held &held) {
  if (!GUI::GET::pressed(page)) return;
  ::carrying.clear();
  for (const GUI::Event &event : GUI::GET::events(page))
    for (const String &id : held.grips)
      if (event.kind == GUI::Event::PRESSED && id == String(event.id))
        ::carrying = id;
}

void landed(const CHANGES::Tape &tape, const CHANGES::Mark &mark) {
  const GUI::Handle page = VIEWS::document();
  const String id = ::grip(mark);
  if (!GUI::GET::moved(page, id.c_str())) return;
  ::carrying.clear();
  const Float place =
    GUI::GET::position(page, id.c_str()).x + CHANGES::HANDLE + ::AIR - mark.east;
  const Whole pulses = tape.pulsed(place);
  const Whole snapped = (pulses + PULSES / 2) / PULSES * PULSES;
  if (snapped == 0 || snapped == mark.at) return;
  mark.move(mark.at, snapped);
}

}  // namespace

void SOUND::VIEWS::CHANGES::handled(
  const Tape &tape, const Vector<Mark> &marks) {
  const GUI::Handle page = document();
  Held &held = ::standing(tape.strip);
  ::rebuilt(page, tape.strip, held, marks);
  ::pressing(page, held);
  for (const Mark &mark : marks) {
    const String id = ::grip(mark);
    ::landed(tape, mark);
    if (id == ::carrying) continue;
    const GUI::Position at = GUI::GET::position(page, mark.id.c_str());
    GUI::set(page, id.c_str(), GUI::Position{at.x - HANDLE - ::AIR, at.y});
  }
}
