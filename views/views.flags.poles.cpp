// SPDX-License-Identifier: AGPL-3.0-or-later
#include "views.flags.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot HANDLE = "grip";
constexpr STRING::Hot PLATE = "panel";
constexpr STRING::Hot MAST = "pole";
constexpr STRING::Hot WORDED = "label";
constexpr STRING::Hot INK = "boxlabel";
constexpr Float ABOVE = 2.0f;
constexpr Float HALVED = 2.0f;
constexpr Float ORIGIN = 0.0f;
constexpr Float LETTER = 11.0f;
constexpr Float INSET = 3.0f;
constexpr Float PAD = 1.0f;

struct Strip {
  GUI::Position at;
  Float deep = 0.0f;
};

auto mast(const String &id) -> String { return id + "." + ::MAST; }

auto measured(const String &strip) -> Strip {
  const GUI::Handle page = VIEWS::document();
  return {
    GUI::GET::position(page, strip.c_str()),
    GUI::GET::measured(page, strip.c_str()).h};
}

void dressed(const String &id, STRING::Hot dress) {
  const GUI::Handle page = VIEWS::document();
  GUI::set(page, id.c_str(), GUI::Style{dress});
  GUI::set(page, id.c_str(), GUI::Depth{::ABOVE});
}

void laid(const String &id, Flag shown, GUI::Position at, GUI::Extent size) {
  const GUI::Handle page = VIEWS::document();
  GUI::set(page, id.c_str(), GUI::Visibility{shown});
  if (id != VIEWS::carried()) GUI::set(page, id.c_str(), at);
  GUI::set(page, id.c_str(), size);
}

}  // namespace

auto SOUND::VIEWS::region(const String &strip) -> String {
  return strip.substr(0, strip.rfind('.'));
}

void SOUND::VIEWS::hoist(
  const String &strip, const String &id, STRING::Hot dress, Flag worded) {
  const GUI::Handle page = document();
  GUI::NODES::create(page, region(strip).c_str(), ::HANDLE, id.c_str());
  ::dressed(id, dress);
  GUI::set(page, id.c_str(), GUI::Axis{.across = true});
  const String pole = ::mast(id);
  GUI::NODES::create(page, id.c_str(), ::PLATE, pole.c_str());
  ::dressed(pole, dress);
  if (!worded) return;
  const String name = id + "." + NAMED;
  GUI::NODES::create(page, id.c_str(), ::WORDED, name.c_str());
  GUI::set(page, name.c_str(), GUI::Style{::INK});
  GUI::set(page, name.c_str(), GUI::Size{::LETTER});
  GUI::set(page, name.c_str(), GUI::Position{::INSET, ::PAD});
}

void SOUND::VIEWS::strike(const String &id) {
  const GUI::Handle page = document();
  GUI::NODES::remove(page, (id + "." + NAMED).c_str());
  GUI::NODES::remove(page, ::mast(id).c_str());
  GUI::NODES::remove(page, id.c_str());
}

void SOUND::VIEWS::fly(
  const String &strip, const String &id, const Flight &flight) {
  const ::Strip seat = ::measured(strip);
  const Float west =
    seat.at.x + flight.place - (flight.closing ? flight.wide : ::ORIGIN);
  const Float north = flight.above ? seat.at.y - HOIST : seat.at.y + seat.deep;
  const Float reach = flight.above ? seat.deep : seat.deep / ::HALVED;
  ::laid(id, flight.shown, {west, north}, {flight.wide, HOIST});
  ::laid(
    ::mast(id), flight.shown,
    {flight.closing ? flight.wide - POLE : ::ORIGIN,
     flight.above ? HOIST : -reach},
    {POLE, reach});
  if (flight.worded)
    GUI::set(
      document(), (id + "." + NAMED).c_str(),
      GUI::Extent{flight.wide - ::INSET * ::HALVED, HOIST});
}

auto SOUND::VIEWS::planted(const String &strip, const String &id, Flag closing)
  -> Float {
  const GUI::Handle page = document();
  const GUI::Position at = GUI::GET::position(page, id.c_str());
  const Float wide = GUI::GET::extent(page, id.c_str()).w;
  return at.x - ::measured(strip).at.x + (closing ? wide : ::ORIGIN);
}
