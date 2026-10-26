// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cmath>

#include <island/gui/nga.hpp>

#include "../../shape.hpp"
#include "signals.bend.hpp"

namespace {
using namespace SOUND;

auto ratio(Float authored, Float compiled) -> Float {
  return authored > 0.0f && compiled > 0.0f ? compiled / authored : 1.0f;
}

auto inside(Float at, Float from, Float span) -> Flag {
  return at >= from && at < from + span;
}

auto on(Whole row, Float x, Float y) -> Flag {
  const Vector<VIEWS::SIGNALS::Point> &rows = VIEWS::SIGNALS::shown();
  const Whole next = VIEWS::SIGNALS::closed(rows, row);
  if (next == NONE || rows[row].index == NONE) return false;
  const Float west = VIEWS::SIGNALS::across(rows[row].at);
  const Float east = VIEWS::SIGNALS::across(rows[row].until);
  if (x < west || x > east || east <= west) return false;
  const Float opens = VIEWS::SIGNALS::down(rows[row].value);
  const Float rise = VIEWS::SIGNALS::down(rows[next].value) - opens;
  const Float part = (x - west) / (east - west);
  const Float stands =
    opens +
    rise * SHAPE::reading(rows[row].shape, part, VIEWS::SIGNALS::bowed(row));
  return std::abs(y - stands) <= VIEWS::SIGNALS::PLATE;
}

auto aboard(GUI::Handle page, STRING::Hot id, GUI::Position spot)
  -> GUI::Position {
  const GUI::Position corner = GUI::GET::origin(page, id);
  const GUI::Extent size = GUI::GET::measured(page, id);
  const GUI::Extent authored = GUI::GET::extent(page, id);
  const GUI::NGA::Zoom zoom = GUI::NGA::GET::zoom(page, id);
  const GUI::NGA::Pan pan = GUI::NGA::GET::pan(page, id);
  const Float sinks = zoom.down > GUI::NGA::Zoom::SAME ? zoom.down : zoom.value;
  return {
    pan.x + (spot.x - corner.x) / (::ratio(authored.w, size.w) * zoom.value),
    pan.y + (spot.y - corner.y) / (::ratio(authored.h, size.h) * sinks)};
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::under(Float x, Float y) -> Whole {
  for (Whole row = 0; row < VIEWS::SIGNALS::shown().size(); ++row)
    if (::on(row, x, y)) return row;
  return NONE;
}

auto SOUND::VIEWS::SIGNALS::hovered() -> Flag {
  const GUI::Handle page = document();
  const String field = board();
  const GUI::Position spot = VIEWS::spot();
  const GUI::Position corner = GUI::GET::origin(page, field.c_str());
  const GUI::Extent size = GUI::GET::measured(page, field.c_str());
  if (
    !::inside(spot.x, corner.x, size.w) || !::inside(spot.y, corner.y, size.h))
    return false;
  const GUI::Position at = ::aboard(page, field.c_str(), spot);
  return under(at.x, at.y) != NONE;
}
