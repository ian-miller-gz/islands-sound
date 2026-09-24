// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../history.hpp"
#include "../inventory.hpp"
#include "../kind.hpp"
#include "render.internal.hpp"

namespace {
using namespace SOUND;

void owing(Whole stock) {
  const Vector<Stock> &stocks = INVENTORY::held().stocks;
  if (stock >= stocks.size() || stocks[stock].kind == KIND::LOGIC)
    return GRAPH::owe();
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    for (const ARRANGEMENT::TRACK::Lane &lane : track.lanes)
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips)
        if (span.stock == stock) GRAPH::owe(track.root);
}

void owing(const Touched &touched) {
  if (touched.whole) return GRAPH::owe();
  for (const Whole stock : touched.stocks) ::owing(stock);
}

}  // namespace

auto SOUND::RENDER::claims() -> Vector<String> {
  ::owing(HISTORY::touched());
  ::owing(INVENTORY::touched());
  HISTORY::settled();
  INVENTORY::settled();
  return GRAPH::owed();
}
