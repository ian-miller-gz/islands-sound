// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot GROUND = "range";
constexpr STRING::Hot PANEL = "panel";

Whole stood = 0;
Whole born = 0;

auto ground() -> String { return VIEWS::SIGNALS::board() + "." + ::GROUND; }

void build(GUI::Handle page, Whole count) {
  const String bed = ::ground();
  if (::stood == 0 && count != 0)
    BOARDS::place(page, VIEWS::SIGNALS::board().c_str(), ::PANEL, bed, {}, {});
  BOARDS::sweep(page, bed.c_str(), count, ::stood);
  for (Whole span = ::stood; span < count; ++span) {
    const String id = GUI::SAC::SEAT::named(bed.c_str(), span);
    BOARDS::place(page, bed.c_str(), ::PANEL, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Depth{VIEWS::SIGNALS::LAID});
  }
  ::stood = count;
}

}  // namespace

void SOUND::VIEWS::SIGNALS::SHED::range() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::stood = 0;
  if (::stood == 0) return;
  ::build(page, 0);
  BOARDS::drop(page, ::ground());
}

void SOUND::VIEWS::SIGNALS::range() {
  const GUI::Handle page = document();
  const Vector<Span> lit = spans();
  if (VIEWS::reborn(::born)) ::stood = 0;
  if (lit.size() != ::stood) ::build(page, lit.size());
  const String bed = ::ground();
  const String skin = dressed(::GROUND);
  for (Whole span = 0; span < lit.size(); ++span) {
    const String id = GUI::SAC::SEAT::named(bed.c_str(), span);
    const Float opens = across(lit[span].opens);
    const Float closes = across(lit[span].closes);
    GUI::set(page, id.c_str(), GUI::Position{opens, NORTH});
    GUI::set(
      page, id.c_str(),
      GUI::Extent{closes > opens ? closes - opens : ORIGIN, DEEP});
    GUI::set(page, id.c_str(), GUI::Style{skin.c_str()});
  }
}

void SOUND::VIEWS::SIGNALS::range(SHELL::Session &session) {
  const Vector<Span> lit = spans();
  session.print(std::format("range spans {}", lit.size()));
  for (const Span &span : lit)
    session.print(std::format(
      "range stock {} opens {} closes {}", span.stock, span.opens,
      span.closes));
}
