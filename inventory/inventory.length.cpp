// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "inventory.doors.hpp"

namespace {
using namespace SOUND;

auto ended(const Vector<Turn> &turns) -> Whole {
  return turns.empty() ? 0 : turns.back().at;
}

auto ended(const Vector<Choice> &choices) -> Whole {
  return choices.empty() ? 0 : choices.back().at;
}

auto ended(const Vector<Point> &points) -> Whole {
  return points.empty() ? 0 : points.back().at;
}

auto ended(const Vector<Note> &notes) -> Whole {
  Whole last = 0;
  for (const Note &note : notes) last = std::max(last, note.at + note.length);
  return last;
}

}  // namespace

auto SOUND::INVENTORY::length(const Stock &stock) -> Whole {
  if (stock.kind == KIND::AUDIO)
    return stock.take.lanes.empty() ? 0 : stock.take.lanes[0].size();
  if (stock.kind == KIND::LOGIC) return ::ended(stock.curve.points);
  if (stock.kind == KIND::CONTROL) return ::ended(stock.turns);
  if (stock.kind == KIND::PROGRAM) return ::ended(stock.choices);
  return ::ended(stock.score.notes);
}
