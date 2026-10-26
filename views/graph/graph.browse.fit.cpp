// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "graph.browse.crop.hpp"
#include "graph.browse.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::WIRED::BROWSE;

constexpr STRING::Hot PATTERN = "browserow";
constexpr Float EDGE = 10.0f;

constexpr STRING::Hot HOME = "graph";
Float authored = 0.0f;

auto titled(const Offer &offer) -> String {
  return offer.directory ? std::format("{} {}", offer.name, INTO) : offer.name;
}

auto part(GUI::Handle page, STRING::Hot cell) -> Float {
  return GUI::GET::position(page, (String(::PATTERN) + "." + cell).c_str()).x;
}

auto fitted(GUI::Handle page, const Vector<Offer> &rows) -> Fit {
  Float name = 0.0f, reading = 0.0f;
  for (const Offer &offer : rows) {
    name = std::max(name, Float(::titled(offer).size()));
    reading = std::max(reading, Float(crossed(offer).size()));
  }
  const Float advance = VIEWS::LETTERS::ADVANCE;
  const Float named = ::part(page, VIEWS::WIRED::NAME);
  const Float across =
    std::max(::part(page, CROSSING), named + name * advance * SIZED + AIR);
  const GUI::Position seat = GUI::GET::position(page, PANEL);
  const GUI::Extent home = GUI::GET::measured(page, ::HOME);
  const Float run = across + reading * advance * READ + RIM;
  const Fit whole{across, across - named - AIR, std::max(run, ::authored)};
  if (home.w <= 0.0f) return whole;
  return cut(whole, run, named, home.w + seat.x - ::EDGE);
}

}  // namespace

auto SOUND::VIEWS::WIRED::BROWSE::crossed(const Offer &offer) -> String {
  if (offer.directory) return std::to_string(offer.holds);
  return std::format("{} {} {}", filed(offer), UNDER, stated(offer));
}

void SOUND::VIEWS::WIRED::BROWSE::listing(
  GUI::Handle page, const Vector<Offer> &rows) {
  if (::authored <= 0.0f) ::authored = GUI::GET::extent(page, PANEL).w;
  const Fit fit = ::fitted(page, rows);
  GUI::set(page, PANEL, GUI::Extent{fit.wide, GUI::GET::extent(page, PANEL).h});
  GUI::set(page, ROWS, GUI::Rows{rows.size()});
  const Whole first = GUI::GET::first(page, ROWS);
  for (Whole row = 0; first + row < rows.size(); ++row) {
    const String cell = std::format("{}.{}", ROWS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    const Offer &offer = rows[first + row];
    crop(page, cell, ::titled(offer), crossed(offer), fit);
  }
}
