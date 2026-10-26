// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/tip.hpp>

#include "../views.letters.hpp"
#include "rig.hpp"

namespace {
using namespace SOUND;
namespace RIG = SOUND::VIEWS::RIG;

constexpr Float SIDES = 2.0f;
constexpr Float SHUT = 0.0f;

auto ring(GUI::Handle page, STRING::Hot id, const GUI::Extent &box) -> Float {
  if (GUI::GET::dial(page, id).graphic != GUI::Dial::DIAL) return 0.0f;
  return box.h - GUI::GET::border(page, id);
}

auto run(GUI::Handle page, STRING::Hot id) -> Float {
  const GUI::Extent box = GUI::GET::extent(page, id);
  return box.w - ::ring(page, id, box) - ::SIDES * GUI::GET::pad(page, id);
}

void name(GUI::Handle page, const String &cell, const RIG::Source &source) {
  const String id = cell + "." + RIG::NAMED;
  const String whole = source.name;
  const String shown = VIEWS::LETTERS::cropped(
    whole, GUI::GET::extent(page, id.c_str()).w, RIG::LETTER);
  GUI::set(page, id.c_str(), GUI::Text{shown});
  GUI::TIP::note(page, id.c_str(), shown == whole ? String() : whole);
}

void open(GUI::Handle page, const String &dial, const String &whole) {
  if (String(GUI::GET::editing(page)) != dial) return;
  const String run = GUI::GET::text(page, dial.c_str());
  if (run == whole || run != RIG::fitted(page, dial, whole)) return;
  GUI::set(page, dial.c_str(), GUI::Text{whole});
  GUI::set(page, dial.c_str(), GUI::Caret{.at = whole.size(), .from = 0});
}

void typed(
  GUI::Handle page, const String &dial, const RIG::Source &source,
  const AUDIO::PLUGIN::Control &published) {
  ::open(page, dial, RIG::worded(published, source.read()));
  if (!GUI::GET::committed(page, dial.c_str())) return;
  GUI::edit(page, "");
  Float value = 0.0f;
  if (RIG::parsed(published, GUI::GET::text(page, dial.c_str()), value))
    source.write(value);
}

}  // namespace

auto SOUND::VIEWS::RIG::fitted(
  GUI::Handle page, const String &dial, const String &whole) -> String {
  const String cut = LETTERS::cropped(
    whole, ::run(page, dial.c_str()), GUI::GET::size(page, dial.c_str()));
  GUI::TIP::note(page, dial.c_str(), cut == whole ? String() : whole);
  return cut;
}

void SOUND::VIEWS::RIG::turn(
  GUI::Handle page, const String &cell, const Source &source, Float column) {
  const String dial = cell + "." + DIALLED;
  const AUDIO::PLUGIN::Control published = source.published();
  seat(page, cell, column, GUI::GET::measured(page, cell.c_str()).w, ::SHUT);
  ::name(page, cell, source);
  GUI::set(page, dial.c_str(), dressed(page, published));
  ::typed(page, dial, source, published);
  if (GUI::GET::dialled(page, dial.c_str()))
    return source.write(GUI::GET::value(page, dial.c_str()));
  const Float value = source.read();
  GUI::set(page, dial.c_str(), GUI::Value{value});
  if (String(GUI::GET::editing(page)) != dial)
    GUI::set(
      page, dial.c_str(),
      GUI::Text{fitted(page, dial, worded(published, value))});
}
