// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../transport.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MARKED = "mark";
constexpr STRING::Hot LABEL = "label";
constexpr STRING::Hot CELL = "entry";
constexpr STRING::Hot INK = "rulerink";
constexpr STRING::Hot BUTTON = "button";
constexpr STRING::Hot PANEL = "panel";
constexpr Float LETTER = 12.0f;
constexpr Float MARGIN = 5.0f;
constexpr Float WORD = 44.0f;

VIEWS::Pool pool;

auto tick(Whole at) -> String {
  return std::format("{}.{}", VIEWS::SIGNALS::strip(), at);
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
  const String strip = VIEWS::SIGNALS::strip();
  for (Whole at = count; at < ::pool.stood; ++at)
    GUI::NODES::remove(page, ::tick(at).c_str());
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::tick(at);
    ::place(page, strip, ::BUTTON, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::CELL});
    const String hair = id + "." + ::MARKED;
    ::place(page, id, ::PANEL, hair, {}, {});
    GUI::set(page, hair.c_str(), GUI::Style{::INK});
    const String said = id + "." + ::LABEL;
    ::place(page, id, ::LABEL, said, {::MARGIN, ::MARGIN}, {::WORD, ::LETTER});
    GUI::set(page, said.c_str(), GUI::Style{::INK});
    GUI::set(page, said.c_str(), GUI::Size{::LETTER});
  }
  VIEWS::built(::pool, count);
}

void written(Whole step, const Vector<GUI::SAC::LADDER::Mark> &dressed) {
  const GUI::Handle page = VIEWS::document();
  const Float scale = VIEWS::SIGNALS::scaled();
  const Float west =
    GUI::NGA::GET::pan(page, VIEWS::SIGNALS::board().c_str()).x;
  const Float deep =
    GUI::GET::measured(page, VIEWS::SIGNALS::strip().c_str()).h;
  for (Whole at = 0; at < dressed.size(); ++at) {
    const String id = ::tick(at);
    const Whole frames = Whole(dressed[at].at);
    GUI::set(
      page, id.c_str(),
      GUI::Position{
        (VIEWS::SIGNALS::across(frames) - west) * scale,
        VIEWS::SIGNALS::NORTH});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{
        VIEWS::SIGNALS::across(VIEWS::spanned(dressed, at, step)) * scale,
        deep});
    GUI::set(
      page, (id + "." + ::MARKED).c_str(),
      GUI::Extent{VIEWS::SIGNALS::HAIR, deep});
    GUI::set(page, (id + "." + ::LABEL).c_str(), GUI::Text{dressed[at].label});
    if (GUI::GET::clicked(page, id.c_str())) TRANSPORT::locate(frames);
  }
}

}  // namespace

void SOUND::VIEWS::SIGNALS::SHED::ruler() {
  VIEWS::reborn(::pool);
  ::build(0);
}

void SOUND::VIEWS::SIGNALS::ruler() {
  const Vector<GUI::SAC::LADDER::Mark> dressed = marks();
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, dressed.size(), window());
  if (seats != ::pool.stood) ::build(seats);
  VIEWS::light(::pool, ::tick, dressed.size());
  ::written(rung(), dressed);
}

void SOUND::VIEWS::SIGNALS::ruler(SHELL::Session &session) {
  const Whole step = rung();
  const Vector<GUI::SAC::LADDER::Mark> dressed = marks();
  session.print(std::format(
    "ruler marks {} span {} first {} beat {} bar {}", ::pool.lit, step,
    dressed.empty() || step == 0 ? 0 : Whole(dressed.front().at) / step, beat(),
    opening().numerator));
}
