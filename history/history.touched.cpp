// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "history.internal.hpp"

namespace {
using namespace SOUND;

Touched held;
Whole counted = 0;

constexpr STRING::Hot BLIND[] = {
  HISTORY::WORD::HOME, HISTORY::WORD::NAME, HISTORY::WORD::TAG};

auto blind(const Change &change) -> Flag {
  for (STRING::Hot word : ::BLIND)
    if (change.word == word) return true;
  return false;
}

auto heard(const Vector<Change> &changes) -> Flag {
  for (const Change &change : changes)
    if (!::blind(change)) return true;
  return false;
}

auto bounded(const Edit &edit) -> Flag {
  return edit.swept.empty() && edit.graph.nodes.empty() &&
         edit.graph.wires.empty() && edit.graph.slack.empty() &&
         edit.placements.empty() && edit.gone.empty() && edit.maps.empty() &&
         !::heard(edit.changes) && edit.rows.empty() && edit.lanes.empty();
}

void stocked(Whole stock) {
  Vector<Whole> &stocks = ::held.stocks;
  if (std::find(stocks.begin(), stocks.end(), stock) == stocks.end())
    stocks.push_back(stock);
}

}  // namespace

auto SOUND::HISTORY::counted() -> Whole { return ::counted; }

auto SOUND::HISTORY::touched() -> const Touched & { return ::held; }

void SOUND::HISTORY::settled() { ::held = {}; }

void SOUND::HISTORY::stir() {
  ++::counted;
  ::held.whole = true;
}

void SOUND::HISTORY::stir(const Edit &edit) {
  ++::counted;
  if (!::bounded(edit)) {
    ::held.whole = true;
    return;
  }
  for (const Noted &note : edit.notes) ::stocked(note.stock);
  for (const Plotted &plot : edit.plots) ::stocked(plot.stock);
}
