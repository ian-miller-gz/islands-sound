// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <limits>

#include "../../graph.hpp"
#include "../views.hpp"

namespace SOUND::VIEWS::ROLL::STAFF {

constexpr Integer TOP = -6, FOOT = 13, STRAYS = 14;
constexpr Integer NOWHERE = std::numeric_limits<Integer>::min();
constexpr Integer LINES[] = {0, 2, 4, 6, 8};
constexpr Float SCALE = 1.0f;
constexpr Float LENGTH = 3.5f, SHORTEST = 2.5f, CAP = 1.0f;

struct Line {
  Integer place = 0;
  Whole note = 0;
  String name;
};

struct Stem {
  Whole at = 0;
  Integer from = 0;
  Float tip = 0.0f;
  Whole beams = 0;
  Flag foot = false;
};

struct Value {
  Whole part = 0, beams = 0;
};
constexpr Value VALUES[] = {{1, 0}, {2, 1}, {4, 2}};

struct Beam {
  Whole stem = 0, rank = 0, to = NONE;
};

struct Box {
  GUI::Position at;
  GUI::Extent size;
};

namespace GET {

auto lines() -> const Vector<Line> &;
auto rules() -> Vector<Integer>;

auto stems() -> const Vector<Stem> &;

}  // namespace GET

auto placed(Whole pitch) -> Integer;

auto stepped(Whole pitch, Integer by) -> Whole;

auto placed(
  const Vector<AUDIO::PLUGIN::Voice> &voices,
  const Vector<Whole> &pitches) -> Vector<Line>;
auto gridded(const Vector<Line> &lines) -> Vector<Line>;

auto relined() -> Flag;

void lined(SHELL::Session &session);

auto lane(Whole pitch, Whole at) -> Float;

auto pitch(Float y, Whole at) -> Whole;

auto deep() -> Float;

void drawn();
void ruled(const Vector<Integer> &rules);

auto spanned() -> GUI::Extent;

auto beaten(Whole pulse) -> Whole;

void stemmed();

void beamed(Flag moved);

auto joined(const Vector<Stem> &stems) -> Vector<Beam>;
auto sloped(Vector<Stem> &stems, const Vector<Beam> &beams) -> void;

auto boxed(const Stem &stem) -> Box;

namespace SHED {
void drawn();
void stemmed();
void beamed();
}  // namespace SHED

}  // namespace SOUND::VIEWS::ROLL::STAFF

namespace SOUND::VIEWS::ROLL {

auto spelled(Whole pitch) -> String;

}  // namespace SOUND::VIEWS::ROLL
