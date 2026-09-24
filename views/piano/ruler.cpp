// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/ladder.hpp>
#include <island/gui/nga.hpp>
#include <island/gui/playhead.hpp>

#include "../../transport.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MARKED = "mark";
constexpr STRING::Hot LABELED = "label";
constexpr STRING::Hot CELL = "entry";
constexpr STRING::Hot INK = "rulerink";
constexpr STRING::Hot HEADED = "head";
constexpr STRING::Hot CAPPED = "rulerhead";
constexpr STRING::Hot BEATS = "beat";
constexpr STRING::Hot BARS = "bar";
constexpr Float HAIR = 2.0f;
constexpr Float LETTER = 12.0f;
constexpr Float INSET = 5.0f;
constexpr Float PAD = 4.0f;
constexpr Float CAP = 6.0f;
constexpr Float OVER = 1.0f;
constexpr Float NORTH = 0.0f;
constexpr Float CROWD = 44.0f;
constexpr Whole COARSER = 2;

VIEWS::Pool pool;
Flag capping = false;

auto tick(Whole at) -> String {
  return std::format("{}.{}", VIEWS::ROLL::strip(), at);
}

auto cell(const String &id, STRING::Hot part) -> String {
  return id + "." + part;
}

auto barred(Whole pulses) -> String {
  return VIEWS::numbered(VIEWS::ROLL::sections(), Integer(pulses));
}

auto shown() -> Float {
  return VIEWS::ROLL::window().w * Float(VIEWS::ROLL::GRAIN);
}

auto ladder(Whole bar) -> Vector<GUI::SAC::LADDER::Rung> {
  Vector<GUI::SAC::LADDER::Rung> rungs;
  const Float seen = ::shown();
  for (Whole span = PULSES; span > 0 && span < bar; span *= ::COARSER)
    rungs.push_back({span, ::BEATS, ::barred});
  for (Whole span = bar; span > 0; span *= ::COARSER) {
    rungs.push_back({span, ::BARS, ::barred});
    if (Float(span) >= seen) break;
  }
  return rungs;
}

auto west() -> Float {
  return GUI::NGA::GET::pan(VIEWS::document(), VIEWS::ROLL::board().c_str()).x;
}

auto rung() -> GUI::SAC::LADDER::Rung {
  return GUI::SAC::LADDER::climb(
    ::ladder(VIEWS::ROLL::opening().bar),
    VIEWS::ROLL::scaled() / Float(VIEWS::ROLL::GRAIN), ::CROWD);
}

auto depth(GUI::Handle page) -> Float {
  return GUI::GET::measured(page, VIEWS::ROLL::strip().c_str()).h;
}

void place(
  GUI::Handle page, const String &parent, STRING::Hot kind, const String &id,
  GUI::Position at, GUI::Extent size) {
  GUI::NODES::create(page, parent.c_str(), kind, id.c_str());
  GUI::set(page, id.c_str(), at);
  GUI::set(page, id.c_str(), size);
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  const String strip = VIEWS::ROLL::strip();
  for (Whole at = count; at < ::pool.stood; ++at)
    GUI::NODES::remove(page, ::tick(at).c_str());
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::tick(at);
    ::place(page, strip, "button", id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::CELL});
    const String mark = ::cell(id, ::MARKED);
    ::place(page, id, "panel", mark, {}, {});
    GUI::set(page, mark.c_str(), GUI::Style{::INK});
    const String label = ::cell(id, ::LABELED);
    ::place(page, id, "label", label, {::INSET, ::PAD}, {::CROWD, ::LETTER});
    GUI::set(page, label.c_str(), GUI::Style{::INK});
    GUI::set(page, label.c_str(), GUI::Size{::LETTER});
  }
  VIEWS::built(::pool, count);
}

void dress(
  const GUI::SAC::LADDER::Rung &step,
  const Vector<GUI::SAC::LADDER::Mark> &dressed) {
  const GUI::Handle page = VIEWS::document();
  const String board = VIEWS::ROLL::board();
  const Float scale = GUI::NGA::GET::zoom(page, board.c_str()).value;
  const Float west = GUI::NGA::GET::pan(page, board.c_str()).x;
  const Float deep = ::depth(page);
  for (Whole at = 0; at < dressed.size(); ++at) {
    const String id = ::tick(at);
    const Float place = dressed[at].at < 0
                          ? Float(dressed[at].at) / Float(VIEWS::ROLL::GRAIN)
                          : VIEWS::ROLL::across(Whole(dressed[at].at));
    GUI::set(page, id.c_str(), GUI::Position{(place - west) * scale, ::NORTH});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{
        VIEWS::ROLL::across(VIEWS::spanned(dressed, at, step.span)) * scale,
        deep});
    GUI::set(page, ::cell(id, ::MARKED).c_str(), GUI::Extent{::HAIR, deep});
    GUI::set(page, ::cell(id, ::LABELED).c_str(), GUI::Text{dressed[at].label});
  }
}

void located(const Vector<GUI::SAC::LADDER::Mark> &dressed) {
  const GUI::Handle page = VIEWS::document();
  for (Whole at = 0; at < dressed.size(); ++at)
    if (GUI::GET::clicked(page, ::tick(at).c_str()))
      TRANSPORT::locate(SCORE::framed(
        dressed[at].at < 0 ? 0 : Whole(dressed[at].at),
        TRANSPORT::held().tempos));
}

void capped() {
  const GUI::Handle page = VIEWS::document();
  const String id = VIEWS::ROLL::strip() + "." + ::HEADED;
  if (!::capping) {
    ::place(page, VIEWS::ROLL::strip(), "panel", id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::CAPPED});
    GUI::set(page, id.c_str(), GUI::Depth{::OVER});
    ::capping = true;
  }
  const String board = VIEWS::ROLL::board();
  const Float scale = GUI::NGA::GET::zoom(page, board.c_str()).value;
  const Float west = GUI::NGA::GET::pan(page, board.c_str()).x;
  const GUI::SAC::PLAYHEAD::Mark mark = GUI::SAC::PLAYHEAD::stripcap(
    VIEWS::ROLL::across(VIEWS::ROLL::opened(VIEWS::clock())), west, scale,
    ::CAP);
  GUI::set(page, id.c_str(), GUI::Position{mark.at, ::NORTH});
  GUI::set(page, id.c_str(), GUI::Extent{mark.wide, ::depth(page)});
}

}  // namespace

auto SOUND::VIEWS::ROLL::sections() -> Vector<TRANSPORT::Section> {
  return TRANSPORT::sections();
}

auto SOUND::VIEWS::ROLL::opening() -> TRANSPORT::Section {
  const Float pan = ::west() * Float(GRAIN);
  return VIEWS::standing(sections(), pan <= 0.0f ? 0 : Whole(pan));
}

auto SOUND::VIEWS::ROLL::marks() -> Vector<GUI::SAC::LADDER::Mark> {
  const Float grain = Float(GRAIN);
  return VIEWS::dressed(
    sections(), ::ladder, scaled() / grain, ::CROWD, ::west() * grain,
    window().w * grain);
}

void SOUND::VIEWS::ROLL::SHED::ruler() {
  if (VIEWS::reborn(::pool)) {
    ::capping = false;
    return;
  }
  ::build(0);
}

void SOUND::VIEWS::ROLL::ruler() {
  const GUI::SAC::LADDER::Rung step = ::rung();
  const Vector<GUI::SAC::LADDER::Mark> dressed = marks();
  if (VIEWS::reborn(::pool)) ::capping = false;
  const Whole seats = VIEWS::sized(::pool, dressed.size(), window().w);
  if (seats != ::pool.stood) ::build(seats);
  VIEWS::light(::pool, ::tick, dressed.size());
  ::dress(step, dressed);
  ::located(dressed);
  ::capped();
  cycle(step, dressed);
}

void SOUND::VIEWS::ROLL::ruler(SHELL::Session &session) {
  const GUI::SAC::LADDER::Rung step = ::rung();
  const Vector<GUI::SAC::LADDER::Mark> dressed = marks();
  session.print(std::format(
    "ruler rung {} span {} beat {} bar {}", String(step.name), step.span,
    PULSES, opening().numerator));
  session.print(std::format(
    "ruler marks {} first {}", ::pool.lit,
    dressed.empty() ? Integer(0) : dressed.front().at));
}
