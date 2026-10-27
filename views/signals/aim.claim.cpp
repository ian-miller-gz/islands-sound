// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../history.hpp"
#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "../lane/lane.face.hpp"
#include "signals.plotted.hpp"

namespace {
using namespace SOUND;
using Clip = ARRANGEMENT::TRACK::LANE::Clip;

constexpr STRING::Hot CLAIMED = "clip";

auto placed(const VIEWS::SIGNALS::Aim &aim, Whole track, Whole lane) -> Whole {
  const Vector<Clip> &clips = TIMELINE::held().tracks[track].lanes[lane].clips;
  for (Whole row = 0; row < clips.size(); ++row)
    if (clips[row].stock == aim.stock && clips[row].at == aim.opens) return row;
  return NONE;
}

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
  INVENTORY::length(stock, closes - opens);
  const Clip span = {
    .stock = stock, .at = opens, .from = 0, .frames = closes - opens};
  const Whole index = TIMELINE::place(INVENTORY::held(), track, lane, span);
  if (index != NONE)
    HISTORY::record(
      {.act = ::CLAIMED,
       .placements = {
         {.track = track, .lane = lane, .placement = index, .span = span}}});
  return aimed(at);
}

auto SOUND::VIEWS::SIGNALS::extended(const Aim &aim, Whole at) -> Flag {
  const Whole track = attended(), lane = laned();
  if (aim.stock == NONE || track == NONE || lane == NONE) return false;
  const Whole row = ::placed(aim, track, lane);
  Whole opens = 0, closes = 0;
  if (row == NONE || !::barred(at, opens, closes)) return false;
  const Clip was = TIMELINE::held().tracks[track].lanes[lane].clips[row];
  if (closes <= was.at + was.frames) return false;
  Clip shaped = was;
  shaped.frames = closes - was.at;
  if (!TIMELINE::span(track, lane, row, shaped)) return false;
  INVENTORY::length(aim.stock, shaped.frames);
  HISTORY::record(
    {.act = ::CLAIMED,
     .verb = Edit::CHANGED,
     .placements = {
       {.track = track,
        .lane = lane,
        .placement = row,
        .span = shaped,
        .was = was}}});
  return true;
}
