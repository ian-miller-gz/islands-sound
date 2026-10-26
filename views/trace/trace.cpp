// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include "../../graph.hpp"
#include "../../timeline.hpp"
#include "trace.internal.hpp"

namespace {
using namespace SOUND;
namespace TRACE = SOUND::VIEWS::TRACE;

constexpr STRING::Hot CHIP = "chip";
constexpr STRING::Hot NAMED = "name";
constexpr STRING::Hot DRESS = "trace";
constexpr Whole HUES = 6;
constexpr Whole TONES = 4;

auto attended() -> String {
  const Whole track = TRACE::attended();
  if (track == NONE) return {};
  const String root = TIMELINE::held().tracks[track].root;
  return GRAPH::rooted(root) ? root : String();
}

auto labelled(const String &node) -> String {
  const String device = VIEWS::device(node);
  if (device.empty()) return VIEWS::named(node);
  return std::format("{} · {}", VIEWS::named(node), device);
}

auto hue(const GRAPH::TRACE::Step &step) -> Whole {
  return step.branch % ::HUES;
}
auto tone(const GRAPH::TRACE::Step &step) -> Whole {
  const Whole depth = step.depth > 0 ? step.depth - 1 : 0;
  return std::min(depth, ::TONES - 1);
}

void nodes(GUI::Handle page) {
  const Vector<GRAPH::TRACE::Step> steps = TRACE::walked();
  GUI::set(page, TRACE::ROWS, GUI::Rows{steps.size()});
  GUI::set(page, TRACE::EMPTY, GUI::Visibility{::attended().empty()});
  const Whole first = GUI::GET::first(page, TRACE::ROWS);
  for (Whole row = 0; first + row < steps.size(); ++row) {
    const String cell = std::format("{}.{}", TRACE::ROWS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    const GRAPH::TRACE::Step &step = steps[first + row];
    GUI::set(
      page, (cell + "." + ::CHIP).c_str(),
      GUI::Style{
        std::format("{}{}{}", ::DRESS, ::hue(step), ::tone(step)).c_str()});
    GUI::set(
      page, (cell + "." + ::NAMED).c_str(), GUI::Text{::labelled(step.node)});
  }
}

void run() {
  TRACE::turned(VIEWS::document());
  TRACE::editing();
  TRACE::read();
}

void state(SHELL::Session &session) {
  const Vector<GRAPH::TRACE::Step> steps = TRACE::walked();
  session.print(std::format("trace rows {}", steps.size()));
  for (Whole row = 0; row < steps.size(); ++row)
    session.print(std::format(
      "row {} node {} branch {} tone {} {}", row, steps[row].node,
      ::hue(steps[row]), ::tone(steps[row]), ::labelled(steps[row].node)));
  if (::attended().empty())
    session.print(
      std::format("empty {}", GUI::GET::text(VIEWS::document(), TRACE::EMPTY)));
  TRACE::aimed(session);
  TRACE::lanes(session);
  TRACE::clips(session);
  TRACE::editing(session);
}

[[maybe_unused]] const Flag offered = VIEWS::offer(
  {.name = TRACE::NAME, .panel = true, .run = ::run, .state = ::state});

}  // namespace

auto SOUND::VIEWS::TRACE::attended() -> Whole {
  const Whole track = VIEWS::cursor(LIST);
  return track < TIMELINE::held().tracks.size() ? track : NONE;
}
void SOUND::VIEWS::TRACE::nodes() {
  const GUI::Handle page = document();
  ::nodes(page);
  aimed(page);
}

auto SOUND::VIEWS::TRACE::walked() -> Vector<GRAPH::TRACE::Step> {
  Vector<GRAPH::TRACE::Step> steps = GRAPH::TRACE::trace(::attended());
  if (!steps.empty()) steps.erase(steps.begin());
  std::stable_sort(
    steps.begin(), steps.end(),
    [](const GRAPH::TRACE::Step &one, const GRAPH::TRACE::Step &two) {
      return one.branch != two.branch ? one.branch < two.branch
                                      : one.depth < two.depth;
    });
  return steps;
}
