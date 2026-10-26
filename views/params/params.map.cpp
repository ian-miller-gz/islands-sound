// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "params.internal.hpp"
#include "params.map.hpp"

namespace {
using namespace SOUND;
using CONTROL::MAP::Binding;

constexpr STRING::Hot DOOR = "params.bindings";
constexpr STRING::Hot PLATE = VIEWS::RACK::MAPPED;
constexpr STRING::Hot ROWS = "params.map.rows";
constexpr STRING::Hot REGION = "params";
constexpr STRING::Hot NUMBER = "number";
constexpr Float FRAME = 12.0f;

Address typing;
Flag seating = false;

auto listed() -> Vector<Binding> {
  Vector<Binding> rows = CONTROL::MAP::held();
  const Address listen = CONTROL::MAP::listening();
  if (!listen.node.empty())
    rows.push_back({.address = listen, .channel = CONTROL::chosen().channel});
  return rows;
}

auto fit(GUI::Handle page, Whole count) -> Whole {
  const Float step =
    GUI::GET::pitch(page, ::ROWS) + GUI::GET::pad(page, ::ROWS);
  const Float top =
    GUI::GET::origin(page, ::PLATE).y - GUI::GET::origin(page, ::REGION).y;
  const Float room = GUI::GET::measured(page, ::REGION).h - top;
  const Float deep = std::min(room, ::FRAME + Float(count) * step);
  GUI::set(page, ::PLATE, GUI::Extent{GUI::GET::extent(page, ::PLATE).w, deep});
  GUI::set(page, ::ROWS, GUI::Rows{count});
  return Whole((deep - ::FRAME) / step);
}

void seat(GUI::Handle page, Whole count, Whole held) {
  if (!::seating || count == 0) return;
  GUI::set(page, ::PLATE, GUI::Visibility{true});
  const Whole last = count - 1;
  if (held > 0 && last >= GUI::GET::first(page, ::ROWS) + held)
    GUI::set(page, ::ROWS, GUI::Scroll{last + 1 - held});
  const String number = std::format(
    "{}.{}.{}", ::ROWS, last - GUI::GET::first(page, ::ROWS), ::NUMBER);
  if (GUI::edit(page, number.c_str()) != 0) return;
  const Whole end = GUI::GET::text(page, number.c_str()).size();
  GUI::set(page, number.c_str(), GUI::Caret{.at = end, .from = 0});
  ::seating = false;
}

void release(GUI::Handle page) {
  if (::typing.node.empty()) return;
  if (VIEWS::RACK::listening({::typing.node, ::typing.parameter})) return;
  if (String(GUI::GET::editing(page)).starts_with(::ROWS)) GUI::edit(page, "");
  ::typing = {};
  ::seating = false;
}

}  // namespace

void SOUND::VIEWS::RACK::mapped(GUI::Handle page) {
  ::release(page);
  const Vector<Binding> rows = ::listed();
  GUI::set(page, ::DOOR, GUI::Visibility{!rows.empty()});
  const Flag pressed = GUI::GET::clicked(page, ::DOOR);
  const Flag shown = GUI::GET::visibility(page, ::PLATE);
  GUI::set(page, ::PLATE, GUI::Visibility{!rows.empty() && shown != pressed});
  if (!::seating && !GUI::GET::visibility(page, ::PLATE)) return;
  const Whole held = ::fit(page, rows.size());
  map(page, ::ROWS, rows);
  ::seat(page, rows.size(), held);
}

void SOUND::VIEWS::RACK::mapped(SHELL::Session &session) {
  session.print(std::format(
    "map rows {} shown {}", ::listed().size(),
    GUI::GET::visibility(VIEWS::document(), ::PLATE)));
}

void SOUND::VIEWS::RACK::type(const Address &address) {
  CONTROL::MAP::arm(address);
  ::typing = address;
  ::seating = true;
}
