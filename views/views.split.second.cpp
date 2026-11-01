// SPDX-License-Identifier: AGPL-3.0-or-later
#include <utility>

#include <common.hpp>

#include "../graph.hpp"

#include "views.controls.hpp"
#include "views.internal.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;

constexpr Float UNMEASURED = 0.0f;

GUI::Handle page = GUI::NONE;
GUI::Handle lent = GUI::NONE;
Vector<String> roots;
Vector<Byte> bytes;
String name;
FIELDS::Map stood;
Flag fresh = false;

void revived() {
  if (GUI::GET::generation(::page) != 0) return;
  ::page = GUI::load(::name.c_str(), ::bytes);
  ::fresh = true;
}

void keep() {
  const VIEWS::View &unit = VIEWS::registry()[VIEWS::chosen()];
  const String board = VIEWS::GET::board(unit);
  const GUI::Extent laid = GUI::GET::measured(::page, board.c_str());
  if (::fresh && laid.w <= ::UNMEASURED) return;
  if (std::exchange(::fresh, false)) VIEWS::SPLIT::boarded(::stood);
  VIEWS::SPLIT::board(::stood);
}

}  // namespace

void SOUND::VIEWS::SPLIT::mount(STRING::Hot assets, STRING::Hot layout) {
  if (::page != GUI::NONE) GUI::remove(::page);
  ::name = String(assets) + LOWER;
  ::bytes = IO::read(String(assets) + layout);
  ::roots = read(::bytes);
  ::page = GUI::load(::name.c_str(), ::bytes);
}

auto SOUND::VIEWS::SPLIT::second() -> GUI::Handle { return ::page; }

auto SOUND::VIEWS::SPLIT::doubled() -> Flag {
  const Whole at = lower();
  return at != NONE && at == chosen() && mirrored(at) && !hosted(at);
}

auto SOUND::VIEWS::SPLIT::turned() -> Flag {
  return doubled() && focused() == BELOW;
}

auto SOUND::VIEWS::SPLIT::home() -> GUI::Handle {
  return ::lent == GUI::NONE ? sheet() : ::lent;
}

SOUND::VIEWS::SPLIT::Turn::Turn(Flag wanted)
    : on(wanted && ::page != GUI::NONE && ::lent == GUI::NONE) {
  if (!on) return;
  ::lent = sheet();
  sheet() = ::page;
}

SOUND::VIEWS::SPLIT::Turn::~Turn() {
  if (!on) return;
  sheet() = ::lent;
  ::lent = GUI::NONE;
}

void SOUND::VIEWS::SPLIT::draw(GFX::Pass &pass) {
  if (::page == GUI::NONE) return;
  ::revived();
  const Flag on = doubled();
  GUI::place(::page, on ? pass.viewport : GFX::Viewport{});
  if (!on || registry()[chosen()].run == nullptr) return;
  dress(::page, ::roots);
  {
    const Turn turn(true);
    registry()[chosen()].run();
  }
  ::keep();
  GUI::flush(::page, pass);
}
