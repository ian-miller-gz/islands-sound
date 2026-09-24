// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/nga.hpp>

#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot ZOOMED = "zoom";
constexpr STRING::Hot DOWN = "down";
constexpr STRING::Hot PANNED = "pan";
constexpr STRING::Hot X = "x";
constexpr STRING::Hot Y = "y";

}  // namespace

auto SOUND::VIEWS::KEPT::number(
  const FIELDS::Map &face, const String &key, Float &value) -> Flag {
  const auto found = face.find(key);
  if (found == face.end()) return false;
  if (const Float *held = std::get_if<Float>(&found->second)) value = *held;
  return std::holds_alternative<Float>(found->second);
}

void SOUND::VIEWS::KEPT::board(FIELDS::Map &face, const View &unit) {
  const GUI::Handle page = VIEWS::sheet();
  const String id = String(unit.name) + "." + VIEWS::BOARD;
  const GUI::NGA::Zoom scale = GUI::NGA::GET::zoom(page, id.c_str());
  const GUI::NGA::Pan pan = GUI::NGA::GET::pan(page, id.c_str());
  if (scale.value <= 0.0f) return;
  const String stem = String(unit.name) + ".";
  face[stem + ::ZOOMED] = scale.value;
  face[stem + ::ZOOMED + "." + ::DOWN] = scale.down;
  face[stem + ::PANNED + "." + ::X] = pan.x;
  face[stem + ::PANNED + "." + ::Y] = pan.y;
}

void SOUND::VIEWS::KEPT::boarded(const FIELDS::Map &face, const View &unit) {
  const GUI::Handle page = VIEWS::sheet();
  const String id = String(unit.name) + "." + VIEWS::BOARD;
  const String stem = String(unit.name) + ".";
  GUI::NGA::Zoom scale = GUI::NGA::GET::zoom(page, id.c_str());
  GUI::NGA::Pan pan = GUI::NGA::GET::pan(page, id.c_str());
  const Flag scaled = VIEWS::KEPT::number(face, stem + ::ZOOMED, scale.value);
  VIEWS::KEPT::number(face, stem + ::ZOOMED + "." + ::DOWN, scale.down);
  const Flag panned =
    VIEWS::KEPT::number(face, stem + ::PANNED + "." + ::X, pan.x);
  VIEWS::KEPT::number(face, stem + ::PANNED + "." + ::Y, pan.y);
  if (scaled) GUI::NGA::set(page, id.c_str(), scale);
  if (panned) GUI::NGA::set(page, id.c_str(), pan);
}
