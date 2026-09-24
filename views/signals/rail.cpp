// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../boards.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LABEL = "label";
constexpr STRING::Hot PANEL = "panel";
constexpr STRING::Hot INK = "rulerink";
constexpr STRING::Hot NOTHING = "none";
constexpr Whole STRIDE = 16;
constexpr Whole SPOKEN = 32;
constexpr Float LETTER = 12.0f;
constexpr Float MARGIN = 5.0f;
constexpr Float WORD = 26.0f;

Whole stood = 0;
Whole born = 0;

auto marked() -> Vector<Whole> {
  Vector<Whole> values;
  for (Whole value = 0; value < VIEWS::SIGNALS::FULL; value += ::STRIDE)
    values.push_back(value);
  values.push_back(VIEWS::SIGNALS::FULL);
  return values;
}

auto mark(Whole at) -> String {
  return std::format("{}.{}", VIEWS::SIGNALS::column(), at);
}

auto said(Whole value) -> String {
  if (value % ::SPOKEN != 0 && value != VIEWS::SIGNALS::FULL) return {};
  const String word = VIEWS::SIGNALS::spoken(Float(value));
  return value == 0 ? word + VIEWS::SIGNALS::unit() : word;
}

void seat(GUI::Handle page, const Vector<Whole> &values) {
  const String strip = VIEWS::SIGNALS::column();
  for (Whole at = 0; at < values.size(); ++at) {
    const String id = ::mark(at);
    BOARDS::place(page, strip.c_str(), ::PANEL, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    const String number = id + "." + ::LABEL;
    BOARDS::place(page, strip.c_str(), ::LABEL, number, {}, {::WORD, ::LETTER});
    GUI::set(page, number.c_str(), GUI::Style{::INK});
    GUI::set(page, number.c_str(), GUI::Size{::LETTER});
  }
  ::stood = values.size();
}

}  // namespace

void SOUND::VIEWS::SIGNALS::SHED::rail() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::stood = 0;
  for (Whole at = 0; at < ::stood; ++at) {
    const String id = ::mark(at);
    BOARDS::drop(page, id + "." + ::LABEL);
    BOARDS::drop(page, id);
  }
  ::stood = 0;
}

void SOUND::VIEWS::SIGNALS::rail() {
  const GUI::Handle page = document();
  const Vector<Whole> values = ::marked();
  if (VIEWS::reborn(::born)) ::stood = 0;
  if (::stood != values.size()) ::seat(page, values);
  const Float north = GUI::NGA::GET::pan(page, board().c_str()).y;
  const Float scale = sunk();
  const Float run = GUI::GET::measured(page, column().c_str()).w;
  for (Whole at = 0; at < ::stood; ++at) {
    const String id = ::mark(at);
    const Float line = (down(Float(values[at])) - north) * scale;
    GUI::set(page, id.c_str(), GUI::Position{ORIGIN, line});
    GUI::set(page, id.c_str(), GUI::Extent{run, HAIR});
    const String number = id + "." + ::LABEL;
    GUI::set(page, number.c_str(), GUI::Position{::MARGIN, line + HAIR});
    GUI::set(page, number.c_str(), GUI::Text{::said(values[at])});
  }
}

void SOUND::VIEWS::SIGNALS::rail(SHELL::Session &session) {
  Whole numbered = 0;
  for (const Whole value : ::marked())
    numbered += ::said(value).empty() ? 0 : 1;
  session.print(std::format(
    "rail marks {} numbered {} stride {} span {}", ::stood, numbered, ::STRIDE,
    FULL));
  const String suffix = unit();
  session.print(std::format(
    "rail says floor {} ceiling {} unit {}", ::said(0), ::said(FULL),
    suffix.empty() ? String(::NOTHING) : suffix));
}
