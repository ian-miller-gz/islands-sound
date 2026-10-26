// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../graph.hpp"
#include "graph.keys.hpp"
#include "graph.replace.hpp"

namespace {
using namespace SOUND;
namespace BROWSE = SOUND::VIEWS::WIRED::BROWSE;

Map<GUI::Handle, String> replaced;

auto standard(const BROWSE::Offer &row) -> Flag {
  const auto named = [&row](STRING::Hot name) { return row.name == name; };
  return row.directory && std::ranges::any_of(BROWSE::STANDARDS, named);
}

}  // namespace

auto SOUND::VIEWS::WIRED::replaceable(const String &node) -> Flag {
  const Whole at = GRAPH::at(node);
  if (at == NONE || GRAPH::rooted(node)) return false;
  return std::ranges::contains(GRAPH::offers(), GRAPH::held().nodes[at].plugin);
}

void SOUND::VIEWS::WIRED::replacing(const String &node) {
  if (!replaceable(node)) return;
  BROWSE::quick(false);
  BROWSE::enter(String());
  ::replaced[document()] = node;
  GUI::set(document(), NOTICE, GUI::Text{""});
}

auto SOUND::VIEWS::WIRED::replacing() -> String {
  if (!browsing()) ::replaced.erase(document());
  const auto held = ::replaced.find(document());
  return held == ::replaced.end() ? String() : held->second;
}

auto SOUND::VIEWS::WIRED::BROWSE::narrow(
  const String &filing, Vector<Offer> rows) -> Vector<Offer> {
  if (replacing().empty() || !filing.empty()) return rows;
  std::erase_if(rows, [](const Offer &row) { return !::standard(row); });
  return rows;
}

auto SOUND::VIEWS::WIRED::BROWSE::titled() -> String {
  const String node = replacing();
  return node.empty() ? String() : std::format("{} {}", REPLACES, node);
}
