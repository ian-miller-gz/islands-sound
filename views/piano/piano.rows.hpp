// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../../score.hpp"
#include "../views.hpp"

namespace SOUND::VIEWS::ROLL {
struct Keyed;

constexpr Float RAISED = 2.0f;

auto stood(Whole section) -> Float;

void plus();

namespace SHED {
void plus();
}  // namespace SHED

}  // namespace SOUND::VIEWS::ROLL

namespace SOUND::VIEWS::ROLL::ROWS {

constexpr Float OFF = -1.0f;

struct Ladder {
  auto (*lane)(Whole pitch, Whole at) -> Float = nullptr;
  auto (*pitch)(Float y, Whole at) -> Whole = nullptr;
  auto (*deep)() -> Float = nullptr;
};

auto folded() -> Flag;

auto shown(Whole section) -> const Vector<Whole> &;

auto lane(Whole pitch, Whole at) -> Float;

auto pitch(Float y, Whole at) -> Whole;

auto deep() -> Float;

auto added(const Key &key) -> Vector<Whole>;

void add(const Key &key, Whole pitch);

struct Gap {
  Whole low = 0, high = 0;
};

auto gaps(const Key &key) -> Vector<Gap>;

constexpr Integer OCTAVE = 1;

auto named(Whole pitch) -> String;
auto lettered(Whole pitch, const Key &key) -> String;

auto every() -> const Vector<Whole> &;

auto standing(const Vector<Key> &keys) -> Flag;

auto section(const Vector<Key> &keys, Whole at) -> Whole;

auto held(const Vector<Key> &keys, Flag folding) -> Vector<Whole>;

auto ladder(const Vector<Key> &keys, Whole section, const Vector<Whole> &held)
  -> Vector<Whole>;

auto moved(Whole &seen) -> Flag;

struct Anchor {
  Whole pitch = NONE, at = 0;
  Float offset = 0.0f;
};

auto anchored() -> Anchor;
void anchor(const Anchor &held);

void rows();
void rows(SHELL::Session &session);

void banded(GUI::Handle page, STRING::Hot stem, const Vector<Keyed> &sections);

void railed(
  GUI::Handle page, STRING::Hot stem, Whole section, Whole opens, Float x,
  Flag shown);

}  // namespace SOUND::VIEWS::ROLL::ROWS

namespace SOUND::VIEWS::ROLL::PLUS {

constexpr STRING::Hot STEM = "plus";

auto sectioned(Whole section, const ROWS::Gap &gap) -> String;

void pressed(
  GUI::Handle page, Whole section, const Key &key,
  const Vector<ROWS::Gap> &gaps);

void taken();

}  // namespace SOUND::VIEWS::ROLL::PLUS
