// SPDX-License-Identifier: AGPL-3.0-or-later
#include <filesystem>
#include <format>

#include "../../commands.hpp"
#include "../../history.hpp"
#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "../../session.hpp"
#include "../../timeline.hpp"
#include "graph.browse.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LIST = "tracks";
constexpr STRING::Hot WORD = "import";
constexpr STRING::Hot HELP = "lands WAVE";
constexpr STRING::Hot USAGE = "import <path>";
constexpr STRING::Hot ALONE = "no track";
constexpr STRING::Hot CROWDED = "no span";

auto refused(STRING::Hot why) -> String {
  return std::format("{} refused: {}", ::WORD, why);
}

auto named(const String &path) -> String {
  const String stem = std::filesystem::path(path).stem().string();
  const Vector<Stock> &stocks = INVENTORY::held().stocks;
  for (const Stock &stock : stocks)
    if (stock.name == stem) return std::format("{}-{}", stem, stocks.size());
  return stem;
}

auto placed(Whole track, Whole lane, Whole stock) -> Whole {
  const Whole at = TRANSPORT::marker().position;
  const ARRANGEMENT::TRACK::LANE::Clip span = {
    .stock = stock,
    .at = at,
    .from = 0,
    .frames = INVENTORY::length(INVENTORY::held().stocks[stock])};
  COMMANDS::covered(track, lane, at, at + span.frames);
  const Whole index = TIMELINE::place(INVENTORY::held(), track, lane, span);
  if (index == NONE) return NONE;
  HISTORY::record(
    {.act = ::WORD,
     .placements = {
       {.track = track, .lane = lane, .placement = index, .span = span}}});
  return index;
}

void load(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print(::USAGE);
  session.print(VIEWS::WIRED::BROWSE::land(session.arguments[1]));
}

}  // namespace

auto SOUND::VIEWS::WIRED::BROWSE::land(const String &path) -> String {
  const INVENTORY::Heard heard = INVENTORY::heard(path);
  if (heard.refusal[0] != '\0') return ::refused(heard.refusal);
  const Whole track = VIEWS::cursor(::LIST);
  if (track >= TIMELINE::held().tracks.size()) return ::refused(::ALONE);
  const Whole lane = COMMANDS::landing(track, KIND::AUDIO);
  if (lane == NONE) return ::refused(::ALONE);
  const String name = ::named(path);
  const Whole stock = INVENTORY::add(name, KIND::AUDIO);
  const STRING::Hot why = INVENTORY::take(stock, path);
  if (why[0] != '\0') return ::refused(why);
  if (::placed(track, lane, stock) == NONE) return ::refused(::CROWDED);
  return std::format(
    "imported {} on {} {} at {}", name, track, lane,
    TRANSPORT::marker().position);
}

auto SOUND::VIEWS::WIRED::BROWSE::words() -> Vector<SHELL::Command> {
  return {{::WORD, ::HELP, ::load}};
}
