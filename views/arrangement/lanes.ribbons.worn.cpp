// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../control.hpp"
#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "../../render.hpp"
#include "../../timeline.hpp"
#include "lanes.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot WORN = "overridden";
constexpr STRING::Hot STEM = "band";

auto governed(Whole track, Whole lane) -> Flag {
  const String node = RENDER::governs(track, lane);
  for (const Address &address : CONTROL::overrides())
    if (address.node == node) return true;
  return false;
}

auto curved(Whole track, Whole lane) -> Flag {
  const Vector<Stock> &stocks = INVENTORY::held().stocks;
  for (const auto &clip : TIMELINE::held().tracks[track].lanes[lane].clips)
    if (
      clip.stock < stocks.size() &&
      CONTROL::overridden(stocks[clip.stock].curve.address))
      return true;
  return false;
}

struct Read {
  Whole kind = NONE;
  auto (*aside)(Whole track, Whole lane) -> Flag = nullptr;
};

constexpr Read READS[] = {{KIND::CONTROL, ::governed}, {KIND::LOGIC, ::curved}};

auto aside(Whole track, Whole lane) -> Flag {
  const Whole kind = TIMELINE::held().tracks[track].lanes[lane].kind;
  for (const Read &read : ::READS)
    if (read.kind == kind) return read.aside(track, lane);
  return false;
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::overridden() -> Vector<Row> {
  Vector<Row> rows;
  const Vector<SOUND::ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track)
    for (Whole lane = 0; lane < tracks[track].lanes.size(); ++lane)
      if (::aside(track, lane))
        rows.push_back({track, lane, banded(track, lane)});
  return rows;
}

void SOUND::VIEWS::ARRANGEMENT::wear() {
  const GUI::Handle page = VIEWS::document();
  for (const Row &row : overridden()) {
    if (row.band == NONE) continue;
    const String id = std::format("{}.{}.{}", board(), ::STEM, row.band);
    GUI::set(page, id.c_str(), GUI::Style{::WORN});
  }
}

void SOUND::VIEWS::ARRANGEMENT::names(SHELL::Session &session) {
  const Vector<Row> rows = overridden();
  session.print(std::format("ribbon marks {}", rows.size()));
  for (const Row &row : rows)
    session.print(
      std::format("ribbon {} lane {} wears {}", row.track, row.lane, ::WORN));
}
