// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>
#include <island/gui/tip.hpp>

#include "graph.browse.crop.hpp"
#include "graph.browse.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::WIRED::BROWSE;

constexpr STRING::Hot TOLD = "tip {} {}";

void shown(
  GUI::Handle page, const String &id, const String &whole, Float wide,
  Float letter) {
  const String text = VIEWS::LETTERS::cropped(whole, wide, letter);
  GUI::set(page, id.c_str(), GUI::Text{text});
  GUI::TIP::note(page, id.c_str(), text == whole ? String() : whole);
}

}  // namespace

auto SOUND::VIEWS::WIRED::BROWSE::cut(
  const Fit &whole, Float run, Float named, Float room) -> Fit {
  if (run <= room)
    return {whole.across, whole.column, std::min(whole.wide, room)};
  const Float column = SHARE * (room - named - AIR - RIM);
  return {named + column + AIR, column, room};
}

void SOUND::VIEWS::WIRED::BROWSE::crop(
  GUI::Handle page, const String &cell, const String &name,
  const String &crossing, const Fit &fit) {
  const String label = cell + "." + NAME;
  const String reading = cell + "." + CROSSING;
  const Float tall = GUI::GET::extent(page, label.c_str()).h;
  GUI::set(page, label.c_str(), GUI::Extent{fit.column, tall});
  ::shown(page, label, name, fit.column, SIZED);
  const Float down = GUI::GET::position(page, reading.c_str()).y;
  GUI::set(page, reading.c_str(), GUI::Position{fit.across, down});
  ::shown(page, reading, crossing, fit.wide - fit.across - RIM, READ);
}

void SOUND::VIEWS::WIRED::BROWSE::tell(
  SHELL::Session &session, GUI::Handle page) {
  Vector<String> ids = GUI::TIP::GET::notes(page, String(ROWS) + ".");
  std::sort(ids.begin(), ids.end());
  for (const String &id : ids)
    session.print(
      std::format(::TOLD, id, GUI::TIP::GET::note(page, id.c_str())));
}
