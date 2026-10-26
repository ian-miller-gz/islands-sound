// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../inventory.hpp"
#include "../../kind.hpp"
#include "signals.runs.hpp"

namespace {
using namespace SOUND;
using Row = VIEWS::SIGNALS::Row;
using Run = VIEWS::SIGNALS::Run;

auto turned(const Stock &held, Whole index) -> Row {
  if (index >= held.turns.size()) return {};
  const Turn &move = held.turns[index];
  return {move.at, move.value, move.shape};
}

auto chosen(const Stock &held, Whole index) -> Row {
  if (index >= held.choices.size()) return {};
  return {held.choices[index].at, Float(held.choices[index].program)};
}

auto numbered(const Stock &held, Whole at) -> Whole {
  for (Whole index = 0; index < held.turns.size(); ++index)
    if (
      held.turns[index].number == VIEWS::SIGNALS::number() &&
      held.turns[index].at == at)
      return index;
  return NONE;
}

auto placed(const Stock &held, Whole at) -> Whole {
  for (Whole index = 0; index < held.choices.size(); ++index)
    if (held.choices[index].at == at) return index;
  return NONE;
}

auto turn(Whole stock, const Row &laid) -> Flag {
  return INVENTORY::turn(
    stock, {.at = laid.at,
            .number = VIEWS::SIGNALS::number(),
            .value = laid.value,
            .shape = laid.shape});
}

auto choose(Whole stock, const Row &laid) -> Flag {
  return INVENTORY::choose(
    stock, {.at = laid.at, .program = Whole(laid.value)});
}

auto spanned(const Stock &held, Whole opens, Whole closes) -> Vector<Whole> {
  Vector<Whole> rows;
  for (Whole index = 0; index < held.turns.size(); ++index)
    if (
      held.turns[index].number == VIEWS::SIGNALS::number() &&
      held.turns[index].at > opens && held.turns[index].at < closes)
      rows.push_back(index);
  return rows;
}

auto none(const Stock &, Whole, Whole) -> Vector<Whole> { return {}; }

constexpr Run KINDS[] = {
  {KIND::CONTROL, true, ::turned, ::numbered, ::turn, ::spanned},
  {KIND::PROGRAM, false, ::chosen, ::placed, ::choose, ::none},
  {KIND::LOGIC, true, VIEWS::SIGNALS::CURVE::row,
   VIEWS::SIGNALS::CURVE::covered, VIEWS::SIGNALS::CURVE::write,
   VIEWS::SIGNALS::CURVE::between}};

}  // namespace

auto SOUND::VIEWS::SIGNALS::RUNS::paged() -> const Run * {
  for (const Run &run : ::KINDS)
    if (run.kind == SIGNALS::paged()) return &run;
  return nullptr;
}

auto SOUND::VIEWS::SIGNALS::continuous() -> Flag {
  const Run *run = RUNS::paged();
  return run != nullptr && run->continuous;
}
