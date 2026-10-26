// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../arrangement/arrangement.face.hpp"
#include "../lane/lane.face.hpp"
#include "arranger.internal.hpp"

namespace {
using namespace SOUND;

struct Head {
  STRING::Hot word = "";
  const VIEWS::View &(*face)() = nullptr;
};

constexpr Head HEADS[] = {
  {"arrangement", VIEWS::ARRANGEMENT::face},
  {"lane", VIEWS::LANE::face},
  {"piano", VIEWS::LANE::face},
  {"signals", VIEWS::LANE::face}};
constexpr STRING::Hot BOARDS[] = {
  "arrangement.minimap", "arrangement.board", "lane.minimap", "signals.board"};
constexpr char DOT = '.';

const VIEWS::View *held = nullptr;

auto landed(GUI::Handle page) -> String {
  const String over = GUI::GET::hover(page);
  if (!over.empty()) return over;
  for (STRING::Hot board : ::BOARDS)
    if (!GUI::NGA::GET::pressed(page, board).board.empty()) return board;
  return {};
}

auto headed(const String &id) -> String { return id.substr(0, id.find(::DOT)); }

void pressed(GUI::Handle page) {
  if (!GUI::GET::pressed(page)) return;
  const String head = ::headed(::landed(page));
  for (const ::Head &one : ::HEADS)
    if (head == one.word) ::held = &one.face();
}

}  // namespace

void SOUND::VIEWS::ARRANGER::focus(const View &face) { ::held = &face; }

auto SOUND::VIEWS::ARRANGER::focused() -> const View * {
  ::pressed(document());
  return ::held == nullptr ? &LANE::face() : ::held;
}
