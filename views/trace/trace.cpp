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
constexpr STRING::Hot RESTING = "tab";
constexpr STRING::Hot STANDING = "standing";
constexpr STRING::Hot RESTINGWORD = "tabword";
constexpr STRING::Hot STANDINGWORD = "standingword";
constexpr STRING::Hot PLATE = "plate";
constexpr STRING::Hot FOOT = "trace.face.foot";
constexpr Float FOOTED = 1.0f;
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

auto walked() -> Vector<GRAPH::TRACE::Step> {
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

auto hue(const GRAPH::TRACE::Step &step) -> Whole {
  return step.branch % ::HUES;
}
auto tone(const GRAPH::TRACE::Step &step) -> Whole {
  const Whole depth = step.depth > 0 ? step.depth - 1 : 0;
  return std::min(depth, ::TONES - 1);
}

void nodes(GUI::Handle page) {
  const Vector<GRAPH::TRACE::Step> steps = ::walked();
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

void dressed(GUI::Handle page, STRING::Hot tab, Flag standing) {
  const String plate = String(tab) + "." + ::PLATE;
  GUI::set(page, plate.c_str(), GUI::Style{standing ? ::STANDING : ::RESTING});
  GUI::set(page, tab, GUI::Style{standing ? ::STANDINGWORD : ::RESTINGWORD});
}

void turned(GUI::Handle page) {
  if (GUI::GET::clicked(page, TRACE::NODED)) TRACE::laning(false);
  if (GUI::GET::clicked(page, TRACE::LANED)) TRACE::laning(true);
  const Flag laned = TRACE::laning();
  ::dressed(page, TRACE::NODED, !laned);
  ::dressed(page, TRACE::LANED, laned);
  GUI::set(page, ::FOOT, laned ? GUI::NORTHEAST : GUI::NORTHWEST);
  GUI::Position foot = GUI::GET::position(page, ::FOOT);
  foot.x = laned ? -::FOOTED : ::FOOTED;
  GUI::set(page, ::FOOT, foot);
  GUI::set(page, TRACE::ROWS, GUI::Visibility{!laned});
  for (STRING::Hot id : {TRACE::LANES, TRACE::KINDED, TRACE::GROW})
    GUI::set(page, id, GUI::Visibility{laned});
  if (laned) GUI::set(page, TRACE::EMPTY, GUI::Visibility{false});
}

void run() {
  const GUI::Handle page = VIEWS::document();
  ::turned(page);
  TRACE::editing();
  if (TRACE::laning()) return TRACE::lanes();
  ::nodes(page);
}

void state(SHELL::Session &session) {
  const Vector<GRAPH::TRACE::Step> steps = ::walked();
  session.print(std::format("trace rows {}", steps.size()));
  for (Whole row = 0; row < steps.size(); ++row)
    session.print(std::format(
      "row {} node {} branch {} tone {} {}", row, steps[row].node,
      ::hue(steps[row]), ::tone(steps[row]), ::labelled(steps[row].node)));
  if (::attended().empty())
    session.print(
      std::format("empty {}", GUI::GET::text(VIEWS::document(), TRACE::EMPTY)));
  TRACE::lanes(session);
  TRACE::editing(session);
}

[[maybe_unused]] const Flag offered = VIEWS::offer(
  {.name = TRACE::NAME, .panel = true, .run = ::run, .state = ::state});

}  // namespace

auto SOUND::VIEWS::TRACE::attended() -> Whole {
  const Whole track = VIEWS::cursor(LIST);
  return track < TIMELINE::held().tracks.size() ? track : NONE;
}
