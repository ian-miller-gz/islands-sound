// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ROLL::Span;

constexpr STRING::Hot GROUND = "range";
constexpr STRING::Hot LETTERED = "name";
constexpr STRING::Hot INKED = "rangename";
constexpr Float LETTER = 14.0f;
constexpr Float INSET = 5.0f;
constexpr Float PAD = 3.0f;
constexpr Float NAMED = 180.0f;
constexpr Float NORTH = 0.0f;

Whole stood = 0;
Whole born = 0;

auto ground() -> String { return VIEWS::ROLL::board() + "." + ::GROUND; }

auto held(const Span &span) -> Float {
  const GUI::Handle page = VIEWS::document();
  const String board = VIEWS::ROLL::board();
  const Float pan = GUI::NGA::GET::pan(page, board.c_str()).x;
  const Float zoom = GUI::NGA::GET::zoom(page, board.c_str()).value;
  const Float opens = (VIEWS::ROLL::across(span.opens) - pan) * zoom;
  const Float wide =
    (VIEWS::ROLL::across(span.closes) - VIEWS::ROLL::across(span.opens)) * zoom;
  const Float room = std::max(::INSET, wide - ::NAMED - ::INSET);
  return std::clamp(VIEWS::ROLL::RAILED + ::INSET - opens, ::INSET, room);
}

void build(GUI::Handle page, Whole count) {
  const String bed = ::ground();
  if (::stood == 0 && count != 0)
    BOARDS::place(page, VIEWS::ROLL::board().c_str(), "panel", bed, {}, {});
  BOARDS::sweep(page, bed.c_str(), count, ::stood);
  for (Whole span = ::stood; span < count; ++span) {
    const String id = GUI::SAC::SEAT::named(bed.c_str(), span);
    BOARDS::place(page, bed.c_str(), "panel", id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::GROUND});
    GUI::set(page, id.c_str(), GUI::Depth{VIEWS::ROLL::LIT});
    const String name = id + "." + ::LETTERED;
    BOARDS::place(
      page, id.c_str(), "label", name, {::INSET, ::PAD}, {::NAMED, ::LETTER});
    GUI::set(page, name.c_str(), GUI::Style{::INKED});
    GUI::set(page, name.c_str(), GUI::Size{::LETTER});
    GUI::set(page, name.c_str(), GUI::Depth{VIEWS::ROLL::CREST});
    GUI::NGA::set(
      page, name.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::ACROSS});
  }
  ::stood = count;
}

}  // namespace

void SOUND::VIEWS::ROLL::SHED::range() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::stood = 0;
  if (::stood == 0) return;
  ::build(page, 0);
  BOARDS::drop(page, ::ground());
}

void SOUND::VIEWS::ROLL::range() {
  const GUI::Handle page = document();
  const Vector<Span> lit = spans();
  if (VIEWS::reborn(::born)) ::stood = 0;
  if (lit.size() != ::stood) ::build(page, lit.size());
  const String bed = ::ground();
  for (Whole span = 0; span < lit.size(); ++span) {
    const String id = GUI::SAC::SEAT::named(bed.c_str(), span);
    const Float opens = across(lit[span].opens);
    const Float closes = across(lit[span].closes);
    GUI::set(page, id.c_str(), GUI::Position{opens, ::NORTH});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{closes > opens ? closes - opens : 0.0f, DEEP});
    const String name = id + "." + ::LETTERED;
    GUI::set(page, name.c_str(), GUI::Text{lit[span].name});
    GUI::set(page, name.c_str(), GUI::Position{::held(lit[span]), ::PAD});
  }
}

void SOUND::VIEWS::ROLL::range(SHELL::Session &session) {
  const Vector<Span> lit = spans();
  session.print(std::format("range spans {}", lit.size()));
  for (const Span &span : lit)
    session.print(std::format(
      "range stock {} name {} opens {} closes {}", span.stock, span.name,
      span.opens, span.closes));
}
