// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../control.hpp"
#include "../../graph.hpp"
#include "params.internal.hpp"

namespace {
using namespace SOUND;

auto plug(const String &node) -> String {
  const Whole stood = GRAPH::at(node);
  return stood == NONE ? String() : GRAPH::held().nodes[stood].plugin;
}

}  // namespace

auto SOUND::VIEWS::RACK::aimed() -> String {
  const String node = CONTROL::aimed();
  return GRAPH::at(node) == NONE ? String() : node;
}

auto SOUND::VIEWS::RACK::rows() -> Vector<Row> {
  const String aim = aimed();
  Vector<Row> wanted;
  for (const Node &node : GRAPH::held().nodes) {
    if (!aim.empty() && node.name != aim) continue;
    const Whole count = counted(node.name);
    if (aim.empty() && count > 0) wanted.push_back({node.name, NONE});
    for (Whole at = 0; at < count; ++at) wanted.push_back({node.name, at});
  }
  return wanted;
}

auto SOUND::VIEWS::RACK::head(const Row &row) -> Flag {
  return row.parameter == NONE;
}

auto SOUND::VIEWS::RACK::headed(const Row &row) -> String {
  const String name = VIEWS::named(row.node);
  const String worn = ::plug(row.node);
  return worn == name ? name : std::format("{} {}", name, worn);
}

auto SOUND::VIEWS::RACK::named(const Row &row) -> String {
  if (head(row)) return std::format("head {} {}", row.node, ::plug(row.node));
  return std::format("{} {} {}", row.node, VIEWS::named(row.node), titled(row));
}

auto SOUND::VIEWS::RACK::stated(const Row &row) -> String {
  if (head(row)) return named(row);
  const String said = std::format("{} {}", named(row), shown(row));
  return listed(row) ? said + " chooser" : said;
}
