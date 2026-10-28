// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <limits>

#include "../../graph.hpp"
#include "../views.hpp"

namespace SOUND::VIEWS::ROLL::STAFF {

constexpr Integer TOP = -6, FOOT = 13, STRAYS = 14;
constexpr Integer NOWHERE = std::numeric_limits<Integer>::min();
constexpr Integer LINES[] = {0, 2, 4, 6, 8};
// The grand staff's rules: the treble's five, the gap middle C sits in,
// the bass's five — places counted down from the treble's top line, F5.
constexpr Integer GRAND[] = {0, 2, 4, 6, 8, 12, 14, 16, 18, 20};
constexpr Integer REFERENCE = 38;  // F5's diatonic number: place 0
constexpr Integer TREBLE_G = 6, BASS_F = 14;  // the lines the clefs sit on

namespace SIGN {
constexpr Whole SHARP = 0, FLAT = 1, NATURAL = 2, DOUBLE_SHARP = 3,
                DOUBLE_FLAT = 4;
constexpr STRING::Hot GLYPHS[] = {
  "\u266F", "\u266D", "\u266E", "\U0001D12A", "\U0001D12B"};
}  // namespace SIGN

// One pitch spelled under a key: its place on the grand staff, the sign
// it wears before its head (NONE when the signature already says so),
// and its name.
struct Spelt {
  Integer place = 0;
  Whole sign = NONE;
  String name;
};
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

auto spelled(const Key &key, Whole pitch) -> Spelt;
auto pitched(const Key &key) -> Vector<Line>;
auto accidental(Whole pitch) -> Whole;

// The staff's top place: TOP for the drum staff, the highest pitch's
// place (kept even) for the pitched one, so every pitch lands on the
// board.
auto top() -> Integer;

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
