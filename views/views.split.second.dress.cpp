// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <sstream>

#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::SPLIT::MAIN;

constexpr STRING::Hot LEVEL = "  ";
constexpr char BLANK = ' ';
constexpr char NOTE = '#';
constexpr STRING::Hot FRAME = "focus";

void shown(GUI::Handle page, STRING::Hot id, Flag on) {
  if (GUI::GET::visibility(page, id) != on)
    GUI::set(page, id, GUI::Visibility{on});
}

void laid(
  GUI::Handle page, STRING::Hot id, GUI::Position at, GUI::Extent size) {
  const GUI::Position stood = GUI::GET::position(page, id);
  const GUI::Extent held = GUI::GET::extent(page, id);
  if (stood.x != at.x || stood.y != at.y) GUI::set(page, id, at);
  if (held.w != size.w || held.h != size.h) GUI::set(page, id, size);
}

void seated(GUI::Handle page) {
  const GUI::Handle sheet = VIEWS::sheet();
  const Float inset = GUI::GET::style(sheet, ::FRAME).border;
  const GUI::Position at = GUI::GET::position(sheet, MAIN);
  const GUI::Extent margin = GUI::GET::extent(sheet, MAIN);
  const Float top = VIEWS::SPLIT::top();
  ::laid(
    page, MAIN, {at.x + inset, at.y + top + inset},
    {margin.w + inset, std::max(margin.h - top, 0.0f) + inset});
}

void grounded(GUI::Handle page) {
  const GUI::Handle sheet = VIEWS::sheet();
  const STRING::Hot id = VIEWS::SPLIT::GROUND::LOWER;
  ::laid(page, id, GUI::GET::position(sheet, id), GUI::GET::extent(sheet, id));
}

}  // namespace

auto SOUND::VIEWS::SPLIT::read(const Vector<Byte> &bytes) -> Vector<String> {
  std::istringstream file(String(bytes.begin(), bytes.end()));
  const String level(::LEVEL);
  Vector<String> ids;
  for (String line; std::getline(file, line);) {
    if (!line.starts_with(level) || line.size() <= level.size()) continue;
    const char lead = line[level.size()];
    if (lead == ::BLANK || lead == ::NOTE) continue;
    std::istringstream words(line);
    String kind, id;
    if (words >> kind >> id && id != MAIN) ids.push_back(id);
  }
  return ids;
}

void SOUND::VIEWS::SPLIT::dress(GUI::Handle page, const Vector<String> &roots) {
  for (const String &id : roots) ::shown(page, id.c_str(), id == GROUND::LOWER);
  for (const View &unit : roster())
    if (!unit.panel) ::shown(page, unit.name, String(unit.name) == standing());
  ::seated(page);
  ::grounded(page);
}
