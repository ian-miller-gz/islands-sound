// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../score.hpp"
#include "wave.notes.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;
namespace LANE = SOUND::VIEWS::LANE;

constexpr STRING::Hot LINE = "wave notes {}";
constexpr STRING::Hot INK = "found";
constexpr STRING::Hot LETTER = "muted";
constexpr STRING::Hot LEAF = "name";
constexpr STRING::Hot SPELLING = "{}{}";
constexpr Integer OCTAVE = 1;
constexpr Float RAISED = 2.0f;
constexpr Float AIR = 0.2f, FULL = 1.0f;
constexpr Float WIDE = 40.0f, INSET = 4.0f;

struct Look {
  Float scale = 0.0f, deep = 0.0f, depth = 0.0f, size = 0.0f;
};

VIEWS::Pool pool;

auto bar(Whole at) -> String { return std::format("{}.{}", WAVE::NOTE, at); }

auto name(const String &bar) -> String { return bar + "." + ::LEAF; }

void build(GUI::Handle page, Whole count) {
  const String board = WAVE::board();
  for (Whole at = count; at < ::pool.stood; ++at)
    GUI::NODES::remove(page, ::bar(at).c_str());
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::bar(at), name = ::name(id);
    GUI::NODES::create(page, board.c_str(), WAVE::PANEL, id.c_str());
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
    GUI::NODES::create(page, id.c_str(), WAVE::LABEL, name.c_str());
    GUI::set(page, name.c_str(), GUI::Style{::LETTER});
    WAVE::tie(page, id);
  }
  VIEWS::built(::pool, count);
}

void written(
  GUI::Handle page, Whole at, const WAVE::Bar &bar, const Look &look) {
  const String id = ::bar(at), name = ::name(id);
  const Float room = look.deep * (::FULL - ::AIR - ::AIR) - look.depth;
  const Float top = look.deep * ::AIR + room * (::FULL - bar.value);
  GUI::set(page, id.c_str(), GUI::Position{Float(bar.at), top});
  GUI::set(page, id.c_str(), GUI::Extent{Float(bar.run), look.depth});
  WAVE::tie(page, id, Float(bar.run), room * (bar.value - bar.was));
  const Float foot = look.deep - look.size - ::INSET - top;
  GUI::set(page, name.c_str(), GUI::Position{WAVE::ORIGIN, foot});
  GUI::set(page, name.c_str(), GUI::Extent{::WIDE / look.scale, look.size});
  GUI::set(page, name.c_str(), GUI::Size{look.size / look.scale});
  GUI::set(page, name.c_str(), GUI::Text{WAVE::spelled(bar.pitch)});
}

}  // namespace

void SOUND::VIEWS::WAVE::SHED::notes() {
  VIEWS::reborn(::pool);
  ::build(document(), 0);
}

void SOUND::VIEWS::WAVE::notes() {
  const GUI::Handle page = document();
  const Float scale = scaled();
  if (scale <= ORIGIN) return;
  const Float west = GUI::NGA::GET::pan(page, board().c_str()).x;
  const Vector<Bar> laid = bars(west, west + window());
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, laid.size(), window());
  if (seats != ::pool.stood) ::build(page, seats);
  VIEWS::light(::pool, ::bar, laid.size());
  const String named = ::name(NOTE);
  const ::Look look = {
    scale, deep(), GUI::GET::extent(page, NOTE).h,
    GUI::GET::size(page, named.c_str())};
  for (Whole at = 0; at < laid.size(); ++at)
    ::written(page, at, laid[at], look);
}

void SOUND::VIEWS::WAVE::notes(SHELL::Session &session) {
  if (LANE::faced(laned()).notes == 0) return;
  session.print(std::format(::LINE, ::pool.lit));
}

auto SOUND::VIEWS::WAVE::spelled(Whole pitch) -> String {
  return std::format(
    ::SPELLING, SCORE::classed(pitch % CLASSES),
    Integer(pitch / CLASSES) - ::OCTAVE);
}
