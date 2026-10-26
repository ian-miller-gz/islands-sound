// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../transport.hpp"
#include "../views.internal.hpp"
#include "../views.marks.hpp"
#include "wave.internal.hpp"

namespace {
using namespace SOUND;
namespace WAVE = SOUND::VIEWS::WAVE;

constexpr STRING::Hot HAIRLINE = "mark";
constexpr STRING::Hot WORD = "label";
constexpr STRING::Hot CELL = "entry";
constexpr STRING::Hot INK = "rulerink";
constexpr STRING::Hot BUTTON = "button";
constexpr Float LETTER = 12.0f;
constexpr Float MARGIN = 5.0f;
constexpr Float WIDE = 44.0f;
constexpr Float HAIR = 2.0f;

VIEWS::Pool pool;

auto tick(Whole at) -> String {
  return std::format("{}.{}", WAVE::strip(), at);
}

void seat(
  GUI::Handle page, const String &parent, STRING::Hot kind, const String &id,
  STRING::Hot style) {
  GUI::NODES::create(page, parent.c_str(), kind, id.c_str());
  GUI::set(page, id.c_str(), GUI::Style{style});
}

void build(GUI::Handle page, Whole count) {
  const String strip = WAVE::strip();
  for (Whole at = count; at < ::pool.stood; ++at)
    GUI::NODES::remove(page, ::tick(at).c_str());
  for (Whole at = ::pool.stood; at < count; ++at) {
    const String id = ::tick(at);
    ::seat(page, strip, ::BUTTON, id, ::CELL);
    ::seat(page, id, WAVE::PANEL, id + "." + ::HAIRLINE, ::INK);
    const String said = id + "." + ::WORD;
    ::seat(page, id, WAVE::LABEL, said, ::INK);
    GUI::set(page, said.c_str(), GUI::Position{::MARGIN, ::MARGIN});
    GUI::set(page, said.c_str(), GUI::Extent{::WIDE, ::LETTER});
    GUI::set(page, said.c_str(), GUI::Size{::LETTER});
  }
  VIEWS::built(::pool, count);
}

void written(
  GUI::Handle page, Whole at, const Vector<GUI::SAC::LADDER::Mark> &dressed,
  Whole step) {
  const Float scale = WAVE::scaled();
  const Float west = GUI::NGA::GET::pan(page, WAVE::board().c_str()).x;
  const Float deep = GUI::GET::measured(page, WAVE::strip().c_str()).h;
  const String id = ::tick(at);
  const Whole frames = Whole(dressed[at].at);
  const Float run = Float(VIEWS::spanned(dressed, at, step)) * scale;
  GUI::set(
    page, id.c_str(),
    GUI::Position{(Float(frames) - west) * scale, WAVE::ORIGIN});
  GUI::set(page, id.c_str(), GUI::Extent{run, deep});
  GUI::set(page, (id + "." + ::HAIRLINE).c_str(), GUI::Extent{::HAIR, deep});
  GUI::set(page, (id + "." + ::WORD).c_str(), GUI::Text{dressed[at].label});
  if (GUI::GET::clicked(page, id.c_str())) TRANSPORT::locate(frames);
}

}  // namespace

void SOUND::VIEWS::WAVE::SHED::ruler() {
  VIEWS::reborn(::pool);
  ::build(document(), 0);
}

void SOUND::VIEWS::WAVE::ruler() {
  const GUI::Handle page = document();
  const Vector<GUI::SAC::LADDER::Mark> dressed = marks();
  VIEWS::reborn(::pool);
  const Whole seats = VIEWS::sized(::pool, dressed.size(), window());
  if (seats != ::pool.stood) ::build(page, seats);
  VIEWS::light(::pool, ::tick, dressed.size());
  const Whole step = rung();
  for (Whole at = 0; at < dressed.size(); ++at)
    ::written(page, at, dressed, step);
  VIEWS::asked(strip(), dressed, step, VIEWS::pulsed, VIEWS::OFFER::WAVE);
}
