// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../views.internal.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot CELL = "entry";
constexpr Float OVER = 1.0f;

VIEWS::Pool pool;

auto cell(Whole at) -> String {
  return std::format("{}.{}.{}", VIEWS::ARRANGEMENT::strip(), VIEWS::CYCLE, at);
}

auto down() -> Float {
  return GUI::GET::measured(
           VIEWS::document(), VIEWS::ARRANGEMENT::strip().c_str())
           .h -
         VIEWS::BAND;
}

auto placed(Float frames, Float west, Float scale) -> Float {
  return (frames / Float(VIEWS::ARRANGEMENT::GRAIN) - west) * scale;
}

auto unplaced(Float place, Float west, Float scale) -> Whole {
  const Float across = place / scale + west;
  return VIEWS::pulsed(
    across <= 0.0f ? 0 : Whole(across * Float(VIEWS::ARRANGEMENT::GRAIN)));
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  const String strip = VIEWS::ARRANGEMENT::strip();
  for (Whole at = count; at < ::pool.stood; ++at)
    GUI::NODES::remove(page, ::cell(at).c_str());
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::cell(at);
    GUI::NODES::create(page, strip.c_str(), "button", id.c_str());
    GUI::set(page, id.c_str(), GUI::Style{::CELL});
    GUI::set(page, id.c_str(), GUI::Depth{::OVER});
  }
  VIEWS::built(::pool, count);
}

void cells(
  const GUI::SAC::LADDER::Rung &step,
  const Vector<GUI::SAC::LADDER::Mark> &dressed, Float west, Float scale) {
  const GUI::Handle page = VIEWS::document();
  const Float band = ::down();
  for (Whole at = 0; at < dressed.size(); ++at) {
    const String id = ::cell(at);
    const Whole span = VIEWS::spanned(dressed, at, step.span);
    const Float place = ::placed(Float(dressed[at].at), west, scale);
    GUI::set(page, id.c_str(), GUI::Position{place, band});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{VIEWS::ARRANGEMENT::across(span) * scale, VIEWS::BAND});
    if (GUI::GET::clicked(page, id.c_str()))
      VIEWS::cycled(dressed[at].at, span);
  }
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::cycle(
  const GUI::SAC::LADDER::Rung &step,
  const Vector<GUI::SAC::LADDER::Mark> &dressed) {
  const GUI::Handle page = document();
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, dressed.size(), window());
  if (seats != ::pool.stood) ::build(seats);
  VIEWS::light(::pool, ::cell, dressed.size());
  const Float scale = GUI::NGA::GET::zoom(page, board().c_str()).value;
  const Float west = GUI::NGA::GET::pan(page, board().c_str()).x;
  ::cells(step, dressed, west, scale);
  VIEWS::handled(strip(), west, scale, ::unplaced);
  VIEWS::flagged(strip(), west, scale, ::down(), ::placed);
  VIEWS::asked(strip(), dressed, step.span, VIEWS::pulsed);
}

void SOUND::VIEWS::ARRANGEMENT::cycle(SHELL::Session &session) {
  const GUI::Handle page = document();
  const Float scale = GUI::NGA::GET::zoom(page, board().c_str()).value;
  const Float west = GUI::NGA::GET::pan(page, board().c_str()).x;
  VIEWS::flagged(session, ::pool.lit, west, scale, ::placed);
}
