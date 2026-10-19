// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../transport.hpp"
#include "piano.grid.hpp"

void SOUND::VIEWS::ROLL::grid(SHELL::Session &session) {
  const Whole span = GRID::span();
  const Vector<GUI::SAC::LADDER::Mark> marks = GRID::dressed();
  const TRANSPORT::Section one = opening();
  session.print(std::format(
    "grid marks {} beat {} step {} bar {} first {} hair {} metre {}/{}",
    GRID::lit(), PULSES, span / PULSES, one.numerator,
    marks.empty() ? 0 : marks.front().at / Integer(span), GRID::hair(),
    one.numerator, one.denominator));
}
