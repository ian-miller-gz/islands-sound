// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cstdlib>
#include <format>

#include "../history.hpp"
#include "../inventory.hpp"
#include "../kind.hpp"
#include "../score.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;
using Clip = ARRANGEMENT::TRACK::LANE::Clip;

struct Piece {
  Whole placement = NONE;
  Clip span;
  Whole offset = 0, held = 0;
};

auto pulsed(Whole frames) -> Whole {
  return SCORE::pulsed(frames, TRANSPORT::held().tempos);
}

auto pieced(const Vector<Clip> &clips, Vector<Whole> rows, Whole kind)
  -> Vector<::Piece> {
  std::sort(rows.begin(), rows.end(), [&](Whole one, Whole two) {
    return clips[one].at < clips[two].at;
  });
  const Flag noted = kind == KIND::NOTES;
  const Whole first = clips[rows.front()].at;
  Vector<::Piece> pieces;
  for (const Whole row : rows) {
    const Clip &span = clips[row];
    const Whole opens =
      noted ? ::pulsed(span.at) - ::pulsed(first) : span.at - first;
    const Whole held =
      noted ? ::pulsed(span.at + span.frames) - ::pulsed(span.at) : span.frames;
    pieces.push_back({row, span, opens, held});
  }
  return pieces;
}

template <typename Row>
auto inside(const ::Piece &piece, const Row &row) -> Flag {
  return row.at >= piece.span.from && row.at - piece.span.from < piece.held;
}

template <typename Row>
auto moved(const ::Piece &piece, Row row) -> Row {
  row.at = piece.offset + row.at - piece.span.from;
  return row;
}

void scored(Whole stock, const Vector<::Piece> &pieces, const Inventory &pool) {
  Score score;
  for (const ::Piece &piece : pieces)
    for (const Note &note : pool.stocks[piece.span.stock].score.notes)
      if (::inside(piece, note)) score.notes.push_back(::moved(piece, note));
  INVENTORY::score(stock, score);
}

void taken(Whole stock, const Vector<::Piece> &pieces, const Inventory &pool) {
  const ::Piece &last = pieces.back();
  const Whole frames = last.offset + last.held;
  Vector<Vector<Float>> lanes;
  for (const ::Piece &piece : pieces) {
    const Take &take = pool.stocks[piece.span.stock].take;
    if (lanes.size() < take.lanes.size()) lanes.resize(take.lanes.size());
    for (Whole lane = 0; lane < take.lanes.size(); ++lane) {
      lanes[lane].resize(frames, 0.0f);
      for (Whole frame = 0; frame < piece.held; ++frame)
        if (piece.span.from + frame < take.lanes[lane].size())
          lanes[lane][piece.offset + frame] =
            take.lanes[lane][piece.span.from + frame];
    }
  }
  INVENTORY::take(stock, lanes);
}

void curved(Whole stock, const Vector<::Piece> &pieces, const Inventory &pool) {
  INVENTORY::aim(stock, pool.stocks[pieces.front().span.stock].curve.address);
  for (const ::Piece &piece : pieces)
    for (const Point &point : pool.stocks[piece.span.stock].curve.points)
      if (::inside(piece, point)) INVENTORY::plot(stock, ::moved(piece, point));
}

void turned(Whole stock, const Vector<::Piece> &pieces, const Inventory &pool) {
  for (const ::Piece &piece : pieces) {
    for (const Turn &turn : pool.stocks[piece.span.stock].turns)
      if (::inside(piece, turn)) INVENTORY::turn(stock, ::moved(piece, turn));
    for (const Choice &choice : pool.stocks[piece.span.stock].choices)
      if (::inside(piece, choice))
        INVENTORY::choose(stock, ::moved(piece, choice));
  }
}

auto composed(const Vector<::Piece> &pieces, Whole kind) -> Whole {
  const Inventory &pool = INVENTORY::held();
  const Whole stock =
    INVENTORY::add(pool.stocks[pieces.front().span.stock].name, kind);
  if (stock == NONE) return NONE;
  if (kind == KIND::NOTES)
    ::scored(stock, pieces, INVENTORY::held());
  else if (kind == KIND::AUDIO)
    ::taken(stock, pieces, INVENTORY::held());
  else if (kind == KIND::LOGIC)
    ::curved(stock, pieces, INVENTORY::held());
  else
    ::turned(stock, pieces, INVENTORY::held());
  return stock;
}

auto counted(const String &word) -> Whole {
  return std::strtoul(word.c_str(), nullptr, 10);
}

void stripped(
  Whole track, Whole lane, const Vector<Whole> &rows,
  const Vector<Clip> &clips) {
  Vector<Placement> lifted;
  for (const Whole row : rows) lifted.push_back({track, lane, row, clips[row]});
  for (Whole back = rows.size(); back > 0; --back)
    TIMELINE::lift(track, lane, rows[back - 1]);
  HISTORY::record({.act = "join", .verb = Edit::REMOVED, .placements = lifted});
}

}  // namespace

auto SOUND::COMMANDS::joined(Whole track, Whole lane, Vector<Whole> rows)
  -> Whole {
  std::sort(rows.begin(), rows.end());
  rows.erase(std::unique(rows.begin(), rows.end()), rows.end());
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (
    rows.size() < 2 || track >= tracks.size() ||
    lane >= tracks[track].lanes.size())
    return NONE;
  const ARRANGEMENT::TRACK::Lane &held = tracks[track].lanes[lane];
  if (rows.back() >= held.clips.size()) return NONE;
  const Vector<::Piece> pieces = ::pieced(held.clips, rows, held.kind);
  const Whole stock = ::composed(pieces, held.kind);
  if (stock == NONE) return NONE;
  const ::Piece &first = pieces.front(), &last = pieces.back();
  const Clip joined = {
    .stock = stock,
    .at = first.span.at,
    .from = 0,
    .frames = last.span.at + last.span.frames - first.span.at};
  HISTORY::begin();
  HISTORY::said(std::format("join {} clips", rows.size()));
  ::stripped(track, lane, rows, held.clips);
  const Whole placement =
    TIMELINE::place(INVENTORY::held(), track, lane, joined);
  HISTORY::record(
    {.act = "join", .placements = {{track, lane, placement, joined}}});
  HISTORY::end();
  return placement;
}

void SOUND::COMMANDS::join(SHELL::Session &session) {
  if (session.arguments.size() < 5)
    return session.print("join <track> <lane> <placement> <placement> ...");
  const Whole track = ::counted(session.arguments[1]);
  const Whole lane = ::counted(session.arguments[2]);
  Vector<Whole> rows;
  for (Whole at = 3; at < session.arguments.size(); ++at)
    rows.push_back(::counted(session.arguments[at]));
  const Whole placement = COMMANDS::joined(track, lane, rows);
  if (placement == NONE)
    return session.print("join refused: two placements of one lane, at least");
  const Clip &span =
    TIMELINE::held().tracks[track].lanes[lane].clips[placement];
  session.print(std::format(
    "joined {} {} placement {} stock {} at {} for {}", track, lane, placement,
    span.stock, span.at, span.frames));
}
