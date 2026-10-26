// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../control.hpp"
#include "params.overruled.hpp"

namespace {
using namespace SOUND;

struct Worn {
  Flag overridden = false;
  Flag listening = false;
  STRING::Hot style = nullptr;
};

constexpr STRING::Hot KNOB = "knob";
constexpr STRING::Hot HEARING = "hearing";
constexpr STRING::Hot OVERRULED = "overruled";

constexpr Worn WORN[] = {
  {.overridden = false, .listening = false, .style = ::KNOB},
  {.overridden = false, .listening = true, .style = ::HEARING},
  {.overridden = true, .listening = false, .style = ::OVERRULED},
  {.overridden = true, .listening = true, .style = ::HEARING}};

auto worn(Flag overridden, Flag listening) -> STRING::Hot {
  return std::ranges::find_if(
           ::WORN,
           [&](const Worn &row) {
             return row.overridden == overridden && row.listening == listening;
           })
    ->style;
}

}  // namespace

void SOUND::VIEWS::RACK::overruled(
  GUI::Handle page, const String &cell, const Row &row) {
  const Address address = addressed(row);
  const Flag overridden = CONTROL::overridden(address);
  const String dial = cell + "." + DIALLED;
  const String door = cell + "." + RIG::RESTORED;
  GUI::set(page, dial.c_str(), GUI::Style{::worn(overridden, listening(row))});
  GUI::set(page, door.c_str(), GUI::Visibility{overridden});
  if (overridden && GUI::GET::clicked(page, door.c_str()))
    CONTROL::restore(address);
}

void SOUND::VIEWS::RACK::overruled(SHELL::Session &session) {
  for (const Address &address : CONTROL::overrides())
    session.print(
      std::format("overruled {} {}", address.node, address.parameter));
}
