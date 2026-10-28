// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../inventory.hpp"
#include "../views.ends.hpp"
#include "piano.internal.hpp"
#include "piano.rows.hpp"

namespace {
using namespace SOUND;

VIEWS::ENDS::Pool pool;
Whole born = 0;

auto bed() -> VIEWS::ENDS::Bed {
  return {
    VIEWS::ROLL::board(), VIEWS::ROLL::ROWS::deep(), VIEWS::ROLL::scaled(),
    VIEWS::ROLL::GRAIN, VIEWS::ROLL::bench().snap};
}

auto lined() -> Vector<VIEWS::ENDS::Line> {
  const Inventory &pool = INVENTORY::held();
  Vector<VIEWS::ENDS::Line> lines;
  for (const VIEWS::ROLL::Span &span : VIEWS::ROLL::spans()) {
    if (span.stock >= pool.stocks.size() || span.closes <= span.opens)
      continue;
    const Stock &stock = pool.stocks[span.stock];
    const Whole run = span.closes - span.opens;
    const Whole floor = std::min(INVENTORY::carried(stock), run);
    lines.push_back(
      {span.stock, floor, std::min(INVENTORY::length(stock), run),
       VIEWS::ROLL::across(span.opens + floor),
       VIEWS::ROLL::across(run - floor), span.opens});
  }
  return lines;
}

}  // namespace

void SOUND::VIEWS::ROLL::ends() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::pool = {};
  VIEWS::ENDS::lay(page, ::bed(), ::lined(), ::pool);
}

void SOUND::VIEWS::ROLL::SHED::ends() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::pool = {};
  VIEWS::ENDS::bare(page, {VIEWS::ROLL::board()}, ::pool);
}
