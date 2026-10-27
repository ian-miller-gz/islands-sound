// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "../lane/lane.face.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LIST = "tracks";

auto attended() -> Whole {
  const Whole track = VIEWS::cursor(::LIST);
  return track < TIMELINE::held().tracks.size() ? track : NONE;
}

void joined(Whole track, Whole lane, Whole opens, Whole closes) {
  const auto &clips = TIMELINE::held().tracks[track].lanes[lane].clips;
  Vector<Whole> rows;
  for (Whole row = 0; row < clips.size(); ++row)
    if (
      clips[row].at + clips[row].frames == opens || clips[row].at == closes ||
      (clips[row].at == opens && clips[row].frames == closes - opens))
      rows.push_back(row);
  if (rows.size() >= 2) VIEWS::joined(track, lane, rows);
}

auto barred() -> Whole {
  const Metre metre = SCORE::metred(TRANSPORT::held().metres);
  const Whole quarters = metre.denominator == 0
                           ? METRE
                           : metre.numerator * METRE / metre.denominator;
  return (quarters == 0 ? METRE : quarters) * PULSES;
}

void trimmed(Whole track, Whole lane, Whole at, Whole &opens, Whole &closes) {
  for (const ARRANGEMENT::TRACK::LANE::Clip &span :
       TIMELINE::spans(track, lane)) {
    const Whole ends = span.at + span.frames;
    if (ends <= at && ends > opens) opens = ends;
    if (span.at >= at && span.at < closes) closes = span.at;
  }
}

}  // namespace

auto SOUND::VIEWS::ROLL::opened(Whole frames) -> Whole {
  return SCORE::pulsed(frames, TRANSPORT::held().tempos);
}

auto SOUND::VIEWS::ROLL::aimed() -> Aim {
  return aimed(TRANSPORT::marker().position);
}

auto SOUND::VIEWS::ROLL::claimed(Whole pulse) -> Aim {
  return claimed(::attended(), pulse);
}

auto SOUND::VIEWS::ROLL::claimed(Whole track, Whole pulse) -> Aim {
  const Vector<Tempo> &tempos = TRANSPORT::held().tempos;
  const Whole at = SCORE::framed(pulse, tempos);
  if (track >= TIMELINE::held().tracks.size()) return {};
  if (!LANE::GET::admitted(at)) return {};
  const Aim standing = aimed(track, at);
  if (standing.stock != NONE) return standing;
  const Whole lane = laned(track);
  if (lane == NONE) return {};
  const Whole bar = ::barred();
  Whole opens = SCORE::framed(pulse - pulse % bar, tempos);
  Whole closes = SCORE::framed(pulse - pulse % bar + bar, tempos);
  ::trimmed(track, lane, at, opens, closes);
  if (closes <= opens) return {};
  const Whole stock = INVENTORY::add(
    std::format(
      "{}{}", KIND::spoken(KIND::NOTES), INVENTORY::held().stocks.size()),
    KIND::NOTES);
  if (stock == NONE) return {};
  INVENTORY::length(
    stock, SCORE::pulsed(closes, tempos) - SCORE::pulsed(opens, tempos));
  TIMELINE::place(
    INVENTORY::held(), track, lane,
    {.stock = stock, .at = opens, .from = 0, .frames = closes - opens});
  ::joined(track, lane, opens, closes);
  return aimed(track, at);
}

auto SOUND::VIEWS::ROLL::aimed(Whole at) -> Aim {
  return aimed(::attended(), at);
}

void SOUND::VIEWS::ROLL::aimed(SHELL::Session &session) {
  const Aim aim = aimed();
  if (aim.stock == NONE) return session.print("aims nothing");
  session.print(std::format(
    "aims stock {} from {} opens {}", aim.stock, aim.from, aim.opens));
}
