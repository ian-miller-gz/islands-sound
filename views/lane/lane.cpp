// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>
#include <iterator>

#include "../../kind.hpp"
#include "../piano/piano.face.hpp"
#include "../signals/signals.face.hpp"
#include "../wave/wave.face.hpp"
#include "lane.face.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NAME = "lane";
constexpr STRING::Hot TOOLS = "tools";
constexpr Whole EDITABLE[] = {
  KIND::NOTES, KIND::AUDIO, KIND::CONTROL, KIND::PROGRAM, KIND::LOGIC};

struct Faced {
  Whole kind = NONE;
  const VIEWS::View &(*face)() = nullptr;
};

constexpr Faced FACES[] = {
  {KIND::NOTES, VIEWS::ROLL::face}, {KIND::AUDIO, VIEWS::WAVE::face}};

Whole paged = KIND::NOTES;

auto faces() -> Vector<const VIEWS::View *> {
  return {&VIEWS::ROLL::face(), &VIEWS::SIGNALS::face(), &VIEWS::WAVE::face()};
}

void gated(GUI::Handle page) {
  const VIEWS::View *stands = VIEWS::LANE::faced();
  for (const VIEWS::View *unit : ::faces()) {
    const Flag on = unit == stands;
    VIEWS::show(*unit, on);
    GUI::set(
      page, (String(unit->name) + "." + ::TOOLS).c_str(), GUI::Visibility{on});
  }
}

void shed() {
  VIEWS::LANE::SHED::minimap();
  for (const VIEWS::View *unit : ::faces()) VIEWS::shed(*unit);
}

void run() {
  ::gated(VIEWS::document());
  VIEWS::LANE::swap(::paged);
  if (VIEWS::LANE::faced() == &VIEWS::SIGNALS::face())
    VIEWS::SIGNALS::paged(::paged);
  VIEWS::LANE::minimap();
  VIEWS::LANE::faced()->run();
  VIEWS::LANE::bound();
}

void state(SHELL::Session &session) {
  const VIEWS::View *stands = VIEWS::LANE::faced();
  session.print(
    std::format("lane page {} face {}", KIND::spoken(::paged), stands->name));
  VIEWS::LANE::scoped(session);
  if (stands->state != nullptr) stands->state(session);
  VIEWS::LANE::minimap(session);
}

}  // namespace

auto SOUND::VIEWS::LANE::face() -> const View & {
  static const View unit = {
    .name = ::NAME,
    .controls = {ZOOM, PAN},
    .run = ::run,
    .state = ::state,
    .faced = LANE::faced,
    .shed = ::shed};
  return unit;
}

void SOUND::VIEWS::LANE::paged(Whole kind) { ::paged = kind; }

auto SOUND::VIEWS::LANE::editable(Whole kind) -> Flag {
  return std::ranges::find(::EDITABLE, kind) != std::end(::EDITABLE);
}

auto SOUND::VIEWS::LANE::faced() -> const View * { return faced(::paged); }

auto SOUND::VIEWS::LANE::faced(Whole kind) -> const View * {
  for (const ::Faced &row : ::FACES)
    if (row.kind == kind) return &row.face();
  return &SIGNALS::face();
}
