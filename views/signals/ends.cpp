// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../inventory.hpp"
#include "../views.ends.hpp"
#include "signals.internal.hpp"

namespace {
using namespace SOUND;

VIEWS::ENDS::Pool pool;
Whole born = 0;

auto bed() -> VIEWS::ENDS::Bed {
  return {
    VIEWS::SIGNALS::board(), VIEWS::SIGNALS::DEEP, VIEWS::SIGNALS::scaled(),
    VIEWS::SIGNALS::GRAIN};
}

auto lined() -> Vector<VIEWS::ENDS::Line> {
  const Inventory &pool = INVENTORY::held();
  Vector<VIEWS::ENDS::Line> lines;
  for (const VIEWS::SIGNALS::Span &span : VIEWS::SIGNALS::spans()) {
    if (span.stock >= pool.stocks.size() || span.closes <= span.opens)
      continue;
    const Stock &stock = pool.stocks[span.stock];
    const Whole run = span.closes - span.opens;
    const Whole floor = std::min(INVENTORY::carried(stock), run);
    lines.push_back(
      {span.stock, floor, std::min(INVENTORY::length(stock), run),
       VIEWS::SIGNALS::across(span.opens + floor),
       VIEWS::SIGNALS::across(run - floor)});
  }
  return lines;
}

}  // namespace

void SOUND::VIEWS::SIGNALS::ends() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::pool = {};
  VIEWS::ENDS::lay(page, ::bed(), ::lined(), ::pool);
}

void SOUND::VIEWS::SIGNALS::SHED::ends() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::pool = {};
  VIEWS::ENDS::bare(page, ::bed(), ::pool);
}
