// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../../score.hpp"
#include "../../transport.hpp"
#include "../views.internal.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot CHANGE = "change";
constexpr STRING::Hot INK = "icon";
constexpr STRING::Hot ERASES = "Delete";
constexpr STRING::Hot WHOSE = "changes";
constexpr Float LETTER = 11.0f;
constexpr Float WIDE = 96.0f;
constexpr Float NORTH = 1.0f;
constexpr Float INSET = 3.0f;
constexpr Float OVER = 2.0f;

Whole stood = 0;
Whole born = 0;
Whole asking = NONE;

auto mark(Whole at) -> String {
  return std::format("{}.{}.{}", VIEWS::ROLL::strip(), ::CHANGE, at);
}

auto spelled(const Key &key) -> String {
  if (SCORE::degree(key, key.tonic) == NONE) return "off";
  return std::format("{} {}", SCORE::classed(key.tonic), key.scale);
}

void build(GUI::Handle page, Whole count) {
  const String strip = VIEWS::ROLL::strip();
  for (Whole at = count; at < ::stood; ++at)
    GUI::NODES::remove(page, ::mark(at).c_str());
  for (Whole at = ::stood; at < count; ++at) {
    const String id = ::mark(at);
    GUI::NODES::create(page, strip.c_str(), "button", id.c_str());
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    GUI::set(page, id.c_str(), GUI::Size{::LETTER});
    GUI::set(page, id.c_str(), GUI::Depth{::OVER});
    GUI::set(page, id.c_str(), GUI::Extent{::WIDE, VIEWS::BAND + ::INSET});
  }
  ::stood = count;
}

auto asked(const GUI::NGA::Ask &ask) -> Whole {
  if (!ask.asked || !ask.board.empty()) return NONE;
  for (Whole at = 1; at < ::stood; ++at)
    if (ask.target == ::mark(at)) return at;
  return NONE;
}

}  // namespace

void SOUND::VIEWS::ROLL::SHED::changes() {
  if (VIEWS::reborn(::born)) ::stood = 0;
  ::build(document(), 0);
}

void SOUND::VIEWS::ROLL::changes() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::stood = 0;
  const Vector<Key> &keys = TRANSPORT::held().keys;
  if (keys.size() != ::stood) ::build(page, keys.size());
  const Float pan = GUI::NGA::GET::pan(page, board().c_str()).x;
  const Float zoom = GUI::NGA::GET::zoom(page, board().c_str()).value;
  for (Whole at = 0; at < keys.size(); ++at) {
    const String id = ::mark(at);
    GUI::set(page, id.c_str(), GUI::Visibility{at != 0});
    GUI::set(
      page, id.c_str(),
      GUI::Position{(across(keys[at].at) - pan) * zoom + ::INSET, ::NORTH});
    GUI::set(page, id.c_str(), GUI::Text{::spelled(keys[at])});
  }
  if (::asking != NONE) {
    const Whole row = VIEWS::MENU::taken(::WHOSE);
    if (row != NONE && ::asking < keys.size())
      TRANSPORT::unkey(keys[::asking].at);
    if (row != NONE || !VIEWS::MENU::standing()) ::asking = NONE;
    return;
  }
  const GUI::NGA::Ask ask = GUI::NGA::GET::asked(page);
  const Whole change = ::asked(ask);
  if (change == NONE) return;
  ::asking = change;
  VIEWS::MENU::raise({String(::ERASES)}, ask.x, ask.y, ::WHOSE);
}

void SOUND::VIEWS::ROLL::changes(SHELL::Session &session) {
  const Vector<Key> &keys = TRANSPORT::held().keys;
  for (Whole at = 1; at < keys.size(); ++at)
    session.print(std::format(
      "change {} at {} {}", at, keys[at].at,
      GUI::GET::text(document(), ::mark(at).c_str())));
}
