// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../kind.hpp"
#include "../piano/piano.face.hpp"
#include "../signals/signals.face.hpp"
#include "lane.face.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot NAME = "lane";
constexpr STRING::Hot TOOLS = "tools";
constexpr STRING::Hot MAPPED = "minimap";
constexpr STRING::Hot SLIDING = "slide";

Whole paged = KIND::NOTES;

auto faces() -> Vector<const VIEWS::View *> {
  return {&VIEWS::ROLL::face(), &VIEWS::SIGNALS::face()};
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
  if (::paged != KIND::NOTES) VIEWS::SIGNALS::paged(::paged);
  VIEWS::LANE::minimap();
  VIEWS::LANE::faced()->run();
}

void state(SHELL::Session &session) {
  const VIEWS::View *stands = VIEWS::LANE::faced();
  session.print(
    std::format("lane page {} face {}", KIND::spoken(::paged), stands->name));
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

auto SOUND::VIEWS::LANE::faced() -> const View * {
  return ::paged == KIND::NOTES ? &ROLL::face() : &SIGNALS::face();
}

auto SOUND::VIEWS::LANE::map() -> String {
  return String(::NAME) + "." + ::MAPPED;
}

auto SOUND::VIEWS::LANE::slide() -> String {
  return String(::NAME) + "." + ::SLIDING;
}
