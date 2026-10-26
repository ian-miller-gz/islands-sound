// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <charconv>
#include <format>

#include <island/gui/tip.hpp>

#include "params.internal.hpp"
#include "params.map.hpp"

namespace {
using namespace SOUND;
using CONTROL::MAP::Binding;
namespace RACK = VIEWS::RACK;

constexpr STRING::Hot RACKED = "params.rows";
constexpr STRING::Hot HINT = "bindrow.drop.hint";
constexpr STRING::Hot NAME = ".name";
constexpr STRING::Hot CHANNEL = ".channel";
constexpr STRING::Hot NUMBER = ".number";
constexpr STRING::Hot DROP = ".drop";
constexpr Whole COUNTED = 1;

auto spoken(Whole number) -> String {
  return number == NONE ? String() : std::to_string(number);
}

void show(GUI::Handle page, const String &row, const Binding &binding) {
  const Address &address = binding.address;
  const String number = row + ::NUMBER;
  GUI::set(
    page, (row + ::NAME).c_str(),
    GUI::Text{std::format(
      "{} {}", VIEWS::named(address.node),
      RACK::titled({address.node, address.parameter}))});
  GUI::set(
    page, (row + ::CHANNEL).c_str(),
    GUI::Text{std::format("ch {}", binding.channel + ::COUNTED)});
  if (String(GUI::GET::editing(page)) != number)
    GUI::set(page, number.c_str(), GUI::Text{::spoken(binding.number)});
  GUI::TIP::note(page, (row + ::DROP).c_str(), GUI::GET::text(page, ::HINT));
}

void commit(GUI::Handle page, const String &cell, const Address &address) {
  GUI::edit(page, "");
  const String run = GUI::GET::text(page, cell.c_str());
  const char *end = run.data() + run.size();
  Whole number = NONE;
  const auto read = std::from_chars(run.data(), end, number);
  if (read.ec == std::errc() && read.ptr == end && number <= CONTROL::LOUDEST)
    CONTROL::MAP::assign(address, number);
}

void reveal(GUI::Handle page, const Address &address) {
  CONTROL::aim(address.node);
  const Vector<RACK::Row> rows = RACK::rows();
  const auto found =
    std::ranges::find(rows, RACK::Row{address.node, address.parameter});
  if (found == rows.end()) return;
  const Whole row = Whole(found - rows.begin());
  GUI::set(page, ::RACKED, GUI::Rows{rows.size()});
  const Whole first = GUI::GET::first(page, ::RACKED);
  if (row < first || row >= first + GUI::GET::window(page, ::RACKED))
    GUI::set(page, ::RACKED, GUI::Scroll{row});
}

void map(GUI::Handle page, const String &row, const Binding &binding) {
  const String number = row + ::NUMBER;
  if (GUI::GET::committed(page, number.c_str()))
    ::commit(page, number, binding.address);
  if (GUI::GET::clicked(page, (row + ::DROP).c_str()))
    CONTROL::MAP::disarm(binding.address);
  if (GUI::GET::clicked(page, row.c_str())) ::reveal(page, binding.address);
  ::show(page, row, binding);
}

}  // namespace

void SOUND::VIEWS::RACK::map(
  GUI::Handle page, STRING::Hot list, const Vector<Binding> &rows) {
  const Whole first = GUI::GET::first(page, list);
  for (Whole seat = 0; first + seat < rows.size(); ++seat) {
    const String row = std::format("{}.{}", list, seat);
    if (!GUI::GET::visibility(page, row.c_str())) break;
    ::map(page, row, rows[first + seat]);
  }
}
