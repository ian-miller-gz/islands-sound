// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/ladder.hpp>
#include <island/gui/nga.hpp>

#include "../../transport.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot CAPPED = "gutter";
constexpr STRING::Hot GROUND = "ruler";
constexpr STRING::Hot MARKED = "mark";
constexpr STRING::Hot LABELED = "label";
constexpr STRING::Hot CELL = "entry";
constexpr STRING::Hot INK = "rulerink";
constexpr Float HAIR = 2.0f;
constexpr Float LETTER = 12.0f;
constexpr Float INSET = 5.0f;
constexpr Float PAD = 4.0f;
constexpr Float NORTH = 0.0f;

VIEWS::Pool pool;
Flag capping = false;

auto tick(Whole at) -> String {
  return std::format("{}.{}", VIEWS::ARRANGEMENT::strip(), at);
}

auto cell(const String &id, STRING::Hot part) -> String {
  return id + "." + part;
}

auto first(const Vector<GUI::SAC::LADDER::Mark> &marks, Whole span) -> Whole {
  return marks.empty() || span == 0 ? 0 : Whole(marks.front().at) / span;
}

void place(
  GUI::Handle page, const String &parent, STRING::Hot kind, const String &id,
  GUI::Position at, GUI::Extent size) {
  GUI::NODES::create(page, parent.c_str(), kind, id.c_str());
  GUI::set(page, id.c_str(), at);
  GUI::set(page, id.c_str(), size);
}

auto depth(GUI::Handle page) -> Float {
  return GUI::GET::measured(page, VIEWS::ARRANGEMENT::strip().c_str()).h;
}

void capped() {
  const GUI::Handle page = VIEWS::document();
  const String strip = VIEWS::ARRANGEMENT::strip();
  const String id = strip + "." + ::CAPPED;
  if (!::capping) {
    ::place(page, strip, "panel", id, {0.0f, ::NORTH}, {});
    GUI::set(page, id.c_str(), GUI::Style{::GROUND});
    GUI::set(page, id.c_str(), GUI::Depth{VIEWS::ARRANGEMENT::HEADER});
    ::capping = true;
  }
  GUI::set(
    page, id.c_str(), GUI::Extent{VIEWS::ARRANGEMENT::GUTTER, ::depth(page)});
}

void build(Whole count) {
  const GUI::Handle page = VIEWS::document();
  const String strip = VIEWS::ARRANGEMENT::strip();
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
    ::place(
      page, id, "label", label, {::INSET, ::PAD},
      {VIEWS::ARRANGEMENT::CROWD, ::LETTER});
    GUI::set(page, label.c_str(), GUI::Style{::INK});
    GUI::set(page, label.c_str(), GUI::Size{::LETTER});
  }
  VIEWS::built(::pool, count);
}

void dress(
  const GUI::SAC::LADDER::Rung &step,
  const Vector<GUI::SAC::LADDER::Mark> &dressed) {
  const GUI::Handle page = VIEWS::document();
  const String board = VIEWS::ARRANGEMENT::board();
  const Float scale = GUI::NGA::GET::zoom(page, board.c_str()).value;
  const Float west = GUI::NGA::GET::pan(page, board.c_str()).x;
  const Float deep = ::depth(page);
  for (Whole at = 0; at < dressed.size(); ++at) {
    const String id = ::tick(at);
    const Whole frames = Whole(dressed[at].at);
    GUI::set(
      page, id.c_str(),
      GUI::Position{
        (VIEWS::ARRANGEMENT::across(frames) - west) * scale, ::NORTH});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{
        VIEWS::ARRANGEMENT::across(VIEWS::spanned(dressed, at, step.span)) *
          scale,
        deep});
    GUI::set(page, ::cell(id, ::MARKED).c_str(), GUI::Extent{::HAIR, deep});
    GUI::set(page, ::cell(id, ::LABELED).c_str(), GUI::Text{dressed[at].label});
  }
}

void located(const Vector<GUI::SAC::LADDER::Mark> &dressed) {
  const GUI::Handle page = VIEWS::document();
  for (Whole at = 0; at < dressed.size(); ++at)
    if (GUI::GET::clicked(page, ::tick(at).c_str()))
      TRANSPORT::locate(Whole(dressed[at].at));
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::ruler() {
  const GUI::SAC::LADDER::Rung step = rung();
  const Vector<GUI::SAC::LADDER::Mark> dressed = marks();
  if (VIEWS::reborn(::pool)) ::capping = false;
  const Whole seats = VIEWS::sized(::pool, dressed.size(), window());
  if (seats != ::pool.stood) ::build(seats);
  VIEWS::light(::pool, ::tick, dressed.size());
  ::dress(step, dressed);
  ::capped();
  ::located(dressed);
  cap();
  cycle(step, dressed);
}

void SOUND::VIEWS::ARRANGEMENT::ruler(SHELL::Session &session) {
  const GUI::SAC::LADDER::Rung step = rung();
  session.print(std::format(
    "ruler marks {} rung {} span {} first {} beat {} bar {} at {:g}",
    ::pool.lit, String(step.name), step.span, ::first(marks(), step.span),
    beat(), opening().numerator,
    GUI::GET::position(document(), ::tick(0).c_str()).y));
}
