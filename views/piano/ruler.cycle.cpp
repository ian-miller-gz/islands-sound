// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../transport.hpp"
#include "../views.internal.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot CELL = "entry";
constexpr Float OVER = 1.0f;

Whole stood = 0;
Whole born = 0;

auto cell(Whole at) -> String {
  return std::format("{}.{}.{}", VIEWS::ROLL::strip(), VIEWS::CYCLE, at);
}

auto down() -> Float {
  return GUI::GET::measured(VIEWS::document(), VIEWS::ROLL::strip().c_str()).h -
         VIEWS::BAND;
}

auto placed(Float pulses, Float west, Float scale) -> Float {
  return (pulses / Float(VIEWS::ROLL::GRAIN) - west) * scale;
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  const String strip = VIEWS::ROLL::strip();
  for (Whole at = count; at < ::stood; ++at)
    GUI::NODES::remove(page, ::cell(at).c_str());
  for (Whole at = ::stood; at < count; ++at) {
    const String id = ::cell(at);
    GUI::NODES::create(page, strip.c_str(), "button", id.c_str());
    GUI::set(page, id.c_str(), GUI::Style{::CELL});
    GUI::set(page, id.c_str(), GUI::Depth{::OVER});
  }
  ::stood = count;
}

void pressed(Integer at, Whole span) {
  const Integer end = at + Integer(span);
  if (end <= 0) return;
  const Vector<Tempo> &tempos = TRANSPORT::held().tempos;
  const Whole head = SCORE::framed(at < 0 ? 0 : Whole(at), tempos);
  VIEWS::cycled(Integer(head), SCORE::framed(Whole(end), tempos) - head);
}

void cells(
  const GUI::SAC::LADDER::Rung &step,
  const Vector<GUI::SAC::LADDER::Mark> &dressed, Float west, Float scale) {
  const GUI::Handle page = VIEWS::document();
  const Float band = ::down();
  for (Whole at = 0; at < ::stood; ++at) {
    const String id = ::cell(at);
    const Whole span = VIEWS::spanned(dressed, at, step.span);
    const Float place = ::placed(Float(dressed[at].at), west, scale);
    GUI::set(page, id.c_str(), GUI::Position{place, band});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{VIEWS::ROLL::across(span) * scale, VIEWS::BAND});
    if (GUI::GET::clicked(page, id.c_str())) ::pressed(dressed[at].at, span);
  }
}

auto folded(Float frames, Float west, Float scale) -> Float {
  return ::placed(Float(VIEWS::ROLL::opened(Whole(frames))), west, scale);
}

auto pulsed(Whole pulses) -> Whole { return pulses; }

auto unfolded(Float place, Float west, Float scale) -> Whole {
  const Float across = place / scale + west;
  return across <= 0.0f ? 0 : Whole(across * Float(VIEWS::ROLL::GRAIN));
}

}  // namespace

void SOUND::VIEWS::ROLL::SHED::cycle() {
  if (VIEWS::reborn(::born)) ::stood = 0;
  ::build(0);
}

void SOUND::VIEWS::ROLL::cycle(
  const GUI::SAC::LADDER::Rung &step,
  const Vector<GUI::SAC::LADDER::Mark> &dressed) {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) ::stood = 0;
  if (dressed.size() != ::stood) ::build(dressed.size());
  const Float scale = GUI::NGA::GET::zoom(page, board().c_str()).value;
  const Float west = GUI::NGA::GET::pan(page, board().c_str()).x;
  ::cells(step, dressed, west, scale);
  VIEWS::handled(strip(), west, scale, ::unfolded);
  VIEWS::flagged(strip(), west, scale, ::down(), ::folded);
  VIEWS::asked(strip(), dressed, step.span, ::pulsed);
  changes();
}

void SOUND::VIEWS::ROLL::cycle(SHELL::Session &session) {
  const GUI::Handle page = VIEWS::document();
  const Float scale = GUI::NGA::GET::zoom(page, board().c_str()).value;
  const Float west = GUI::NGA::GET::pan(page, board().c_str()).x;
  VIEWS::flagged(session, ::stood, west, scale, ::folded);
  changes(session);
}
