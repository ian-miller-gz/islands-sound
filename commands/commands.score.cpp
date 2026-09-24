// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../inventory.hpp"
#include "../kind.hpp"
#include "../score.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto scored(SHELL::Session &session) -> Whole {
  const Whole index = ::counted(session.arguments[1]);
  if (index >= INVENTORY::held().stocks.size()) {
    session.print(
      std::format("{} refused: no such stock", session.arguments[0]));
    return NONE;
  }
  const Whole kind = INVENTORY::held().stocks[index].kind;
  if (kind != KIND::NOTES) {
    session.print(std::format(
      "{} refused: {} carries no notes", session.arguments[0],
      KIND::spoken(kind)));
    return NONE;
  }
  return index;
}

void taken(Whole stock) {
  HISTORY::take(stock, INVENTORY::held().stocks[stock]);
}

void wrote(STRING::Hot act, Whole stock) {
  HISTORY::wrote(act, stock, INVENTORY::held().stocks[stock]);
}

auto stated(Whole stock, Whole index) -> String {
  const Note &note = INVENTORY::held().stocks[stock].score.notes[index];
  return std::format(
    "note {} in stock {} pitch {} at {} length {} velocity {:.3f}", index,
    stock, note.pitch, note.at, note.length, note.velocity);
}

}  // namespace

void SOUND::COMMANDS::move(SHELL::Session &session) {
  if (session.arguments.size() < 5)
    return session.print("move <stock> <note> <at> <pitch>");
  const Whole stock = ::scored(session);
  if (stock == NONE) return;
  Score score = INVENTORY::held().stocks[stock].score;
  const Whole landed = SCORE::move(
    score, ::counted(session.arguments[2]), ::counted(session.arguments[3]),
    ::counted(session.arguments[4]));
  if (landed == NONE) return session.print("move refused: no such note");
  ::taken(stock);
  INVENTORY::score(stock, score);
  ::wrote("move", stock);
  session.print(::stated(stock, landed));
}

void SOUND::COMMANDS::stretch(SHELL::Session &session) {
  if (session.arguments.size() < 4)
    return session.print("stretch <stock> <note> <length>");
  const Whole stock = ::scored(session);
  if (stock == NONE) return;
  const Whole index = ::counted(session.arguments[2]);
  Score score = INVENTORY::held().stocks[stock].score;
  if (!SCORE::stretch(score, index, ::counted(session.arguments[3])))
    return session.print("stretch refused: no such note, or no length");
  ::taken(stock);
  INVENTORY::score(stock, score);
  ::wrote("stretch", stock);
  session.print(::stated(stock, index));
}

void SOUND::COMMANDS::erase(SHELL::Session &session) {
  if (session.arguments.size() < 3)
    return session.print("erase <stock> <note>");
  const Whole stock = ::scored(session);
  if (stock == NONE) return;
  const Whole index = ::counted(session.arguments[2]);
  Score score = INVENTORY::held().stocks[stock].score;
  if (!SCORE::erase(score, index))
    return session.print("erase refused: no such note");
  ::taken(stock);
  INVENTORY::score(stock, score);
  ::wrote("erase", stock);
  session.print(std::format(
    "erased {} in stock {} notes {}", index, stock,
    INVENTORY::held().stocks[stock].score.notes.size()));
}
