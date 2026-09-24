// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../kind.hpp"
#include "../score.hpp"
#include "../shape.hpp"

namespace SOUND {

struct Address {
  String node;
  Whole parameter = 0;
};

struct Value {
  Address address;
  Float value = 0.0f;
};

struct Take {
  String path;
  Vector<Vector<Float>> lanes;
};

struct Point {
  Whole at = 0;
  Float value = 0.0f;
  Whole shape = SHAPE::LINE;
};

struct Curve {
  Address address;
  Vector<Point> points;
};

struct Turn {
  Whole at = 0;
  Whole number = 0;
  Float value = 0.0f;
  Whole shape = SHAPE::HOLD;
};

struct Choice {
  Whole at = 0;
  Whole program = 0;
};

struct Touched {
  Flag whole = false;
  Vector<Whole> stocks;
};

struct Stock {
  Whole kind = KIND::NOTES;
  String name;
  Score score;
  Take take;
  Curve curve;
  Vector<Turn> turns;
  Vector<Choice> choices;
};

}  // namespace SOUND
