// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../commands.hpp"
#include "../lane/lane.face.hpp"
#include "../views.menu.acts.hpp"
#include "arrangement.internal.hpp"
#include "arrangement.opened.hpp"
#include "keys.internal.hpp"

namespace {
using namespace SOUND;
namespace ARRANGEMENT = SOUND::VIEWS::ARRANGEMENT;
using VIEWS::MENU::Act;

constexpr STRING::Hot EDITS = "Edit";
constexpr STRING::Hot JOINS = "Append";
constexpr STRING::Hot LIFTS = "Delete";
constexpr STRING::Hot SPLITS = "Split";
constexpr STRING::Hot NAMES = "Rename";
constexpr Whole JOINED = 2;

constexpr Whole HALF = 2;

Whole asked = 0;

auto cut(Float across) -> Whole {
  const Float grain = Float(ARRANGEMENT::GRAIN);
  const Whole at = across > 0.0f ? Whole(across * grain) : 0;
  const Whole step = ARRANGEMENT::rung().span;
  if (!ARRANGEMENT::snapped() || step == 0) return at;
  return (at + step / ::HALF) / step * step;
}

auto pieces(Whole row) -> Vector<Whole> {
  const Vector<ARRANGEMENT::Box> &laid = ARRANGEMENT::boxes();
  Vector<Whole> placements;
  for (const Whole one : ARRANGEMENT::chosen())
    if (laid[one].track == laid[row].track && laid[one].lane == laid[row].lane)
      placements.push_back(laid[one].placement);
  return placements;
}

auto editable(Whole row) -> Flag {
  return VIEWS::LANE::editable(ARRANGEMENT::boxes()[row].kind);
}

auto joinable(Whole row) -> Flag { return ::pieces(row).size() >= ::JOINED; }

auto splittable(Whole row) -> Flag {
  const ARRANGEMENT::Box &box = ARRANGEMENT::boxes()[row];
  return ::asked > box.at && ::asked < box.at + box.frames;
}

void edit(Whole row) { ARRANGEMENT::opened(VIEWS::document(), row); }

void join(Whole row) {
  const ARRANGEMENT::Box &box = ARRANGEMENT::boxes()[row];
  COMMANDS::joined(box.track, box.lane, ::pieces(row));
}

void split(Whole row) {
  const ARRANGEMENT::Box &box = ARRANGEMENT::boxes()[row];
  COMMANDS::split(box.track, box.lane, box.placement, ::asked);
  ARRANGEMENT::chosen({});
}

void named(Whole row) { ARRANGEMENT::rename(VIEWS::document(), row); }

void lift(Whole row) {
  const ARRANGEMENT::Box &box = ARRANGEMENT::boxes()[row];
  ARRANGEMENT::lift(box.track, box.lane, box.placement);
}

constexpr Act ACTS[] = {
  {.word = ::EDITS, .act = ::edit, .shown = ::editable},
  {.word = ::JOINS, .act = ::join, .shown = ::joinable},
  {.word = ::LIFTS, .act = ::lift},
  {.word = ::SPLITS, .act = ::split, .shown = ::splittable},
  {.word = ::NAMES, .act = ::named},
};

auto shown(Whole row) -> Vector<const Act *> {
  Vector<const Act *> run;
  if (row >= ARRANGEMENT::boxes().size()) return run;
  for (const Act &act : ::ACTS)
    if (act.shown == nullptr || act.shown(row)) run.push_back(&act);
  return run;
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::BOX::labels(Whole row, Float across)
  -> Vector<String> {
  ::asked = ::cut(across);
  Vector<String> said;
  for (const Act *act : ::shown(row)) said.push_back(String(act->word));
  return said;
}

void SOUND::VIEWS::ARRANGEMENT::BOX::take(Whole row, Whole taken) {
  const Vector<const Act *> run = ::shown(row);
  if (taken < run.size()) run[taken]->act(row);
}
