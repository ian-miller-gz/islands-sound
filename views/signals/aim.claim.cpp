// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "../lane/lane.face.hpp"
#include "signals.plotted.hpp"

namespace {
using namespace SOUND;

void trimmed(Whole at, Whole &opens, Whole &closes) {
  for (const VIEWS::SIGNALS::Span &span : VIEWS::SIGNALS::spans()) {
    if (span.closes <= at && span.closes > opens) opens = span.closes;
    if (span.opens >= at && span.opens < closes) closes = span.opens;
  }
}

auto barred(Whole at, Whole &opens, Whole &closes) -> Flag {
  const TRANSPORT::Section section =
    VIEWS::standing(VIEWS::SIGNALS::sections(), at);
  if (section.bar == 0 || at < section.at) return false;
  opens = section.at + (at - section.at) / section.bar * section.bar;
  closes = opens + section.bar;
  ::trimmed(at, opens, closes);
  return closes > opens;
}

}  // namespace

auto SOUND::VIEWS::SIGNALS::claimed(Whole at) -> Aim {
  const Aim standing = aimed(at);
  if (standing.stock != NONE) return standing;
  const Whole track = attended(), lane = laned(), kind = paged();
  Whole opens = 0, closes = 0;
  if (track == NONE || lane == NONE || !LANE::GET::admitted(at)) return {};
  if (!::barred(at, opens, closes)) return {};
  const Address address = addressed();
  const Whole stock = INVENTORY::add(
    std::format("{}{}", KIND::spoken(kind), INVENTORY::held().stocks.size()),
    kind);
  if (stock == NONE) return {};
  INVENTORY::aim(stock, address);
  TIMELINE::place(
    INVENTORY::held(), track, lane,
    {.stock = stock, .at = opens, .from = 0, .frames = closes - opens});
  return aimed(at);
}
