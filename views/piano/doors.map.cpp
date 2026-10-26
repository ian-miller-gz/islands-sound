// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../graph.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "../views.menu.marked.hpp"
#include "piano.internal.hpp"
#include "piano.notation.hpp"

namespace {
using namespace SOUND;
namespace ROLL = SOUND::VIEWS::ROLL;

constexpr STRING::Hot MAPS = "piano.map";
constexpr STRING::Hot LACKS = "track lacks instrument";
constexpr STRING::Hot MORE = "{} +{}";
constexpr Whole SHOWN = 1;

auto traced() -> Vector<String> {
  const Whole track = VIEWS::LANE::edited(KIND::NOTES).track;
  if (track >= TIMELINE::held().tracks.size()) return {};
  const String &root = TIMELINE::held().tracks[track].root;
  Vector<String> nodes;
  for (const GRAPH::TRACE::Step &step : GRAPH::TRACE::trace(root))
    nodes.push_back(step.node);
  return nodes;
}

auto held(const Vector<String> &names, const String &name) -> Flag {
  return std::ranges::find(names, name) != names.end();
}

void toggle(const String &node) {
  Vector<String> &mapped = ROLL::faced().mapped;
  if (::held(mapped, node))
    std::erase(mapped, node);
  else
    mapped.push_back(node);
}

void raise(const Vector<String> &nodes) {
  const Vector<String> mapped = ROLL::GET::mapped();
  Vector<Flag> ticks;
  for (const String &node : nodes) ticks.push_back(::held(mapped, node));
  VIEWS::MENU::raise(nodes, ::MAPS, ROLL::MAP, ticks);
}

auto said(const Vector<String> &names) -> String {
  if (names.empty()) return ROLL::ABSENT;
  if (names.size() <= ::SHOWN) return names.front();
  return std::format(::MORE, names.front(), names.size() - ::SHOWN);
}

}  // namespace

auto SOUND::VIEWS::ROLL::GET::mapped() -> Vector<String> {
  const Vector<String> nodes = ::traced();
  Vector<String> seated;
  for (const String &name : faced().mapped)
    if (::held(nodes, name)) seated.push_back(name);
  return seated;
}

auto SOUND::VIEWS::ROLL::GET::instruments() -> Vector<String> {
  Vector<String> found;
  for (const String &node : ::traced())
    if (GRAPH::playable(node)) found.push_back(node);
  return found;
}

void SOUND::VIEWS::ROLL::map() {
  const GUI::Handle page = document();
  const Vector<String> nodes = GET::instruments();
  const Whole row = MENU::taken(::MAPS);
  if (row < nodes.size()) ::toggle(nodes[row]);
  GUI::set(page, MAP, GUI::Text{::said(GET::mapped())});
  if (!GUI::GET::clicked(page, MAP)) return;
  if (nodes.empty())
    GUI::set(page, NOTICE, GUI::Text{::LACKS});
  else
    ::raise(nodes);
}
