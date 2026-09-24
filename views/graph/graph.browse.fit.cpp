// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "graph.browse.internal.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::WIRED::BROWSE;

constexpr STRING::Hot PATTERN = "browserow";
constexpr STRING::Hot NAME = "name";
constexpr STRING::Hot CROSSING = "crossing";
constexpr Float SIZED = 15.0f;
constexpr Float READ = 14.0f;
constexpr Float AIR = 12.0f;
constexpr Float RIM = 16.0f;
constexpr Float EDGE = 10.0f;

constexpr STRING::Hot HOME = "graph";
Float authored = 0.0f;

struct Fit {
  Float across = 0.0f;
  Float wide = 0.0f;
};

auto fitted(GUI::Handle page, const Vector<Offer> &rows) -> Fit {
  Float name = 0.0f, reading = 0.0f;
  for (const Offer &offer : rows) {
    name = std::max(name, Float(offer.name.size()));
    reading = std::max(reading, Float(crossed(offer).size()));
  }
  const Float advance = VIEWS::WIRED::ADVANCE;
  const GUI::Position west =
    GUI::GET::position(page, (String(::PATTERN) + "." + ::CROSSING).c_str());
  const GUI::Position named =
    GUI::GET::position(page, (String(::PATTERN) + "." + ::NAME).c_str());
  const Float across =
    std::max(west.x, named.x + name * advance * ::SIZED + ::AIR);
  const GUI::Position seat = GUI::GET::position(page, PANEL);
  const GUI::Extent home = GUI::GET::measured(page, ::HOME);
  Float wide = across + reading * advance * ::READ + ::RIM;
  if (::authored > 0.0f) wide = std::max(wide, ::authored);
  const Float room = home.w + seat.x - ::EDGE;
  if (home.w > 0.0f) wide = std::min(wide, room);
  return {across, wide};
}

}  // namespace

auto SOUND::VIEWS::WIRED::BROWSE::crossed(const Offer &offer) -> String {
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
    const String reading = cell + "." + ::CROSSING;
    GUI::set(page, (cell + "." + ::NAME).c_str(), GUI::Text{offer.name});
    GUI::set(
      page, reading.c_str(),
      GUI::Position{fit.across, GUI::GET::position(page, reading.c_str()).y});
    GUI::set(page, reading.c_str(), GUI::Text{crossed(offer)});
  }
}
