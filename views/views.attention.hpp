// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../score.hpp"
#include "views.hpp"

namespace SOUND::VIEWS::ATTENTION {

constexpr Whole SLOTS = 4;

struct Mark {
  Whole track = NONE, lane = NONE, kind = NONE;
};

struct Slot {
  Whole track = NONE;
  Mark mark;
  Whole clip = NONE;
  String node;
};

auto held(Whole region) -> Whole;

auto hold(Whole region, Whole slot) -> Flag;

auto get(Whole slot) -> Slot &;

auto current() -> Slot &;

auto viewed() -> Slot &;

auto attended(Slot &slot) -> Whole;

auto attended() -> Whole;

auto aimed(Slot &slot) -> String;

auto shown() -> Whole;

void show(Whole slot);

void run(Whole row);

void stand(Whole region);

void frame();

void doors();

void keep(FIELDS::Map &face);
void restore(const FIELDS::Map &face);

}  // namespace SOUND::VIEWS::ATTENTION
