// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../inventory.hpp"
#include "../kind.hpp"
#include "../score.hpp"
#include "../shape.hpp"
#include "commands.internal.hpp"

namespace {
using namespace SOUND;

auto counted(const String &token) -> Whole {
  return std::strtoul(token.c_str(), nullptr, 10);
}

auto valued(const String &token) -> Float {
  return std::strtof(token.c_str(), nullptr);
}

constexpr STRING::Hot AUTHORED = "author";

void taken(Whole index) {
  HISTORY::take(index, INVENTORY::held().stocks[index]);
}

void wrote(Whole index) {
  HISTORY::wrote(::AUTHORED, index, INVENTORY::held().stocks[index]);
}

void noted(SHELL::Session &session, Whole index) {
  if (session.arguments.size() < 6)
    return session.print("author <stock> <pitch> <at> <length> <velocity>");
  const Note note = {
    .pitch = ::counted(session.arguments[2]),
    .at = ::counted(session.arguments[3]),
    .length = ::counted(session.arguments[4]),
    .velocity = ::valued(session.arguments[5])};
  Score score = INVENTORY::held().stocks[index].score;
  const Whole landed = SCORE::write(score, note);
  ::taken(index);
  INVENTORY::score(index, score);
  ::wrote(index);
  session.print(std::format(
    "authored {} in stock {} pitch {} at {} length {} velocity {:.3f}", landed,
    index, note.pitch, note.at, note.length, note.velocity));
}

void turned(SHELL::Session &session, Whole index) {
  if (session.arguments.size() < 5)
    return session.print("author <stock> <number> <at> <value> [<shape>]");
  const Flag stated = session.arguments.size() > 5;
  const Whole shape =
    stated ? SHAPE::meant(session.arguments[5]) : Whole(SHAPE::HOLD);
  if (shape == NONE) return session.print("author refused: no such shape");
  const Turn move = {
    .at = ::counted(session.arguments[3]),
    .number = ::counted(session.arguments[2]),
    .value = ::valued(session.arguments[4]),
    .shape = shape};
  ::taken(index);
  INVENTORY::turn(index, move);
  ::wrote(index);
  session.print(std::format(
    "authored stock {} number {} at {} value {:.3f} turns {}{}", index,
    move.number, move.at, move.value,
    INVENTORY::held().stocks[index].turns.size(),
    stated ? std::format(" shape {}", SHAPE::spoken(shape)) : String()));
}

void chosen(SHELL::Session &session, Whole index) {
  if (session.arguments.size() < 4)
    return session.print("author <stock> <program> <at>");
  const Choice choice = {
    .at = ::counted(session.arguments[3]),
    .program = ::counted(session.arguments[2])};
  ::taken(index);
  INVENTORY::choose(index, choice);
  ::wrote(index);
  session.print(std::format(
    "authored stock {} program {} at {} choices {}", index, choice.program,
    choice.at, INVENTORY::held().stocks[index].choices.size()));
}

}  // namespace

void SOUND::COMMANDS::author(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("author <stock> ...");
  const Whole index = ::counted(session.arguments[1]);
  if (index >= INVENTORY::held().stocks.size())
    return session.print("author refused: no such stock");
  const Whole kind = INVENTORY::held().stocks[index].kind;
  if (kind == KIND::NOTES) return ::noted(session, index);
  if (kind == KIND::CONTROL) return ::turned(session, index);
  if (kind == KIND::PROGRAM) return ::chosen(session, index);
  const STRING::Hot awaited = INVENTORY::refuses(kind);
  if (awaited[0] != '\0')
    return session.print(std::format("author refused: {}", awaited));
  session.print(std::format(
    "author refused: {} is written by {}", KIND::spoken(kind),
    kind == KIND::AUDIO ? "take" : "plot"));
}
