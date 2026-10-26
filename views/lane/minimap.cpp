// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../views.internal.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot GROUND = "ink";
constexpr STRING::Hot TRACED = "trace";
constexpr Float LEDGE = 3.0f;
constexpr Float HAIRLINE = 2.0f;
constexpr Float HALF = 0.5f;
constexpr Float ORIGIN = 0.0f;

VIEWS::Pool pool;

auto ground() -> String { return VIEWS::LANE::map() + "." + ::GROUND; }

auto dash(Whole line) -> String {
  return GUI::SAC::SEAT::named(::ground().c_str(), line);
}

auto folded(Float share, Float deep) -> Float {
  return ::LEDGE +
         share * std::max(::ORIGIN, deep - ::LEDGE * 2.0f - ::HAIRLINE);
}

void build(GUI::Handle page, Whole count) {
  const String bed = ::ground();
  if (::pool.stood == 0 && count != 0)
    BOARDS::place(page, VIEWS::LANE::map().c_str(), "panel", bed, {}, {});
  BOARDS::sweep(page, bed.c_str(), count, ::pool.stood);
  for (Whole line = ::pool.stood; line < count; ++line) {
    const String id = ::dash(line);
    BOARDS::place(page, bed.c_str(), "panel", id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::TRACED});
    GUI::NGA::set(page, id.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::DOWN});
  }
  VIEWS::built(::pool, count);
}

void derive(GUI::Handle page, const String &id, Float wide) {
  const String field = VIEWS::LANE::field();
  const Float scale =
    GUI::NGA::GET::zoom(page, field.c_str()).value * VIEWS::LANE::RATIO;
  if (scale <= ::ORIGIN || wide <= ::ORIGIN) return;
  GUI::NGA::set(page, id.c_str(), GUI::NGA::Zoom{scale});
  if (!GUI::NGA::GET::stroked(page, id.c_str()).board.empty()) return;
  const Float centre = GUI::NGA::GET::pan(page, field.c_str()).x +
                       VIEWS::LANE::windowed() * ::HALF;
  const Float wall = GUI::NGA::GET::bounds(page, field.c_str()).west;
  const Float west = centre - wide / scale * ::HALF;
  GUI::NGA::set(
    page, id.c_str(),
    GUI::NGA::Pan{
      wall == GUI::Walls::NONE ? west : std::max(wall, west), ::ORIGIN});
}

void strolled(GUI::Handle page, const String &id) {
  const GUI::NGA::Ask stroke = GUI::NGA::GET::stroked(page, id.c_str());
  if (stroke.board.empty()) return;
  VIEWS::wandered();
  const String field = VIEWS::LANE::field();
  const GUI::NGA::Pan held = GUI::NGA::GET::pan(page, field.c_str());
  GUI::NGA::set(
    page, field.c_str(),
    GUI::NGA::Pan{stroke.x - VIEWS::LANE::windowed() * ::HALF, held.y});
}

void placed(
  GUI::Handle page, const String &bed, Whole line, const VIEWS::Dash &dash,
  Float scale, Float deep) {
  const String mark = GUI::SAC::SEAT::named(bed.c_str(), line);
  GUI::set(
    page, mark.c_str(), GUI::Position{dash.at, ::folded(dash.deep, deep)});
  GUI::set(
    page, mark.c_str(),
    GUI::Extent{
      scale > ::ORIGIN ? std::max(dash.run, ::HAIRLINE / scale) : dash.run,
      ::HAIRLINE});
}

}  // namespace

void SOUND::VIEWS::LANE::SHED::minimap() {
  const GUI::Handle page = document();
  VIEWS::reborn(::pool);
  if (::pool.stood == 0) return;
  ::build(page, 0);
  BOARDS::drop(page, ::ground());
}

void SOUND::VIEWS::LANE::minimap() {
  const GUI::Handle page = document();
  const String id = map();
  const GUI::Extent strip = GUI::GET::measured(page, id.c_str());
  VIEWS::reborn(::pool);
  ::strolled(page, id);
  ::derive(page, id, strip.w);
  const Overview read = faced()->overview();
  const Whole seats = VIEWS::sized(::pool, read.dashes.size(), windowed());
  if (seats != ::pool.stood) ::build(page, seats);
  VIEWS::light(::pool, ::dash, read.dashes.size());
  const String bed = ::ground();
  const Float scale = GUI::NGA::GET::zoom(page, id.c_str()).value;
  for (Whole line = 0; line < read.dashes.size(); ++line)
    ::placed(page, bed, line, read.dashes[line], scale, strip.h);
  if (read.placed != nullptr) VIEWS::marked(id, read.placed);
  slider();
}

void SOUND::VIEWS::LANE::minimap(SHELL::Session &session) {
  const GUI::Handle page = document();
  const String id = map();
  session.print(std::format(
    "minimap dashes {} reach {} ratio {:g}", ::pool.lit,
    faced()->overview().reach, RATIO));
  session.print(std::format(
    "minimap zoom {:g} pan {:g}", GUI::NGA::GET::zoom(page, id.c_str()).value,
    GUI::NGA::GET::pan(page, id.c_str()).x));
  const Overview read = faced()->overview();
  if (read.placed != nullptr) VIEWS::marked(session, id, read.placed);
}
