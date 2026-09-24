// SPDX-License-Identifier: AGPL-3.0-or-later
#include <sstream>

#include "session.store.internal.hpp"

using namespace SOUND;

namespace {

void addressed(std::istringstream &line, Stock &stock, const Graph &wiring) {
  Whole node = NONE;
  line >> node >> stock.curve.address.parameter;
  stock.curve.address.node = SESSION::named(wiring, node);
}

}  // namespace

void SOUND::SESSION::older(
  const String &text, Arrangement &tracks, const Graph &wiring,
  Vector<Husk> &husks) {
  std::istringstream lines(text);
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag == "t") {
      ARRANGEMENT::Track track;
      Whole root = NONE;
      line >> root;
      track.root = named(wiring, root);
      track.name = rest(line);
      tracks.tracks.push_back(track);
    } else if (tag == "l" || tag == "p") {
      if (!tracks.tracks.empty()) hung(tag, line, tracks.tracks.back());
    } else {
      husked(held, at, husks);
    }
  }
}

void SOUND::SESSION::older(
  const String &text, Inventory &pool, const Graph &wiring,
  Vector<Husk> &husks) {
  std::istringstream lines(text);
  Whole at = 0;
  for (String held; std::getline(lines, held); ++at) {
    std::istringstream line(held);
    String tag;
    line >> tag;
    if (tag == "c") {
      Stock stock;
      line >> stock.kind;
      stock.name = rest(line);
      pool.stocks.push_back(stock);
    } else if (tag == "a") {
      if (!pool.stocks.empty()) ::addressed(line, pool.stocks.back(), wiring);
    } else if (spoken(tag)) {
      if (!pool.stocks.empty()) hung(tag, line, pool.stocks.back());
    } else {
      husked(held, at, husks);
    }
  }
}
