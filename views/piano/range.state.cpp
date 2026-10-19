// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "piano.internal.hpp"

void SOUND::VIEWS::ROLL::range(SHELL::Session &session) {
  const Vector<Span> lit = spans();
  session.print(std::format("range spans {}", lit.size()));
  for (const Span &span : lit)
    session.print(std::format(
      "range stock {} name {} opens {} closes {}", span.stock, span.name,
      span.opens, span.closes));
}
