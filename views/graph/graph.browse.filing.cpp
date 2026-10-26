// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../graph.hpp"
#include "../../kind.hpp"
#include "graph.browse.internal.hpp"

auto SOUND::VIEWS::WIRED::BROWSE::carries(
  const Vector<Whole> &kinds, Whole kind) -> Flag {
  return std::find(kinds.begin(), kinds.end(), kind) != kinds.end();
}

auto SOUND::VIEWS::WIRED::BROWSE::shelved(
  const Vector<Whole> &takes, const Vector<Whole> &gives) -> Whole {
  const Flag played = carries(takes, KIND::NOTES);
  if (carries(gives, KIND::AUDIO)) return played ? INSTRUMENTS : EFFECTS;
  if (carries(gives, KIND::NOTES)) return played ? SHAPERS : CONTROLLERS;
  return PLUGINS;
}

auto SOUND::VIEWS::WIRED::BROWSE::path(const String &plugin) -> String {
  const String type = GRAPH::typed(plugin), voicing = GRAPH::voiced(plugin);
  for (const Filing &filing : FILED) {
    if (type != filing.type) continue;
    const String where = GRAPH::from(plugin) + (WITHIN + String(filing.path));
    const Flag voiced = type == VOICED && !voicing.empty();
    return voiced ? where + WITHIN + voicing : where;
  }
  return {};
}

auto SOUND::VIEWS::WIRED::BROWSE::catalogued() -> Vector<Offer> {
  Vector<Offer> rows;
  for (const String &name : GRAPH::offers()) {
    Offer row = {
      .name = name,
      .from = GRAPH::from(name),
      .takes = GRAPH::takes(name),
      .gives = GRAPH::gives(name)};
    row.shelf = shelved(row.takes, row.gives);
    row.among = path(name);
    rows.push_back(row);
  }
  return rows;
}

auto SOUND::VIEWS::WIRED::among(const String &plugin) -> String {
  for (const BROWSE::Offer &row : BROWSE::catalogued())
    if (row.name == plugin) return BROWSE::filed(row);
  return {};
}
