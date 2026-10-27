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
  Flag overridden = false;
};

struct Take {
  String path;
  Vector<Vector<Float>> lanes;
};

struct Point {
  Whole at = 0;
  Float value = 0.0f;
  Whole shape = SHAPE::LINE;
  Float bend = 0.0f;
  auto operator==(const Point &) const -> bool = default;
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
  Float bend = 0.0f;
  auto operator==(const Turn &) const -> bool = default;
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
  Whole length = 0;
  Score score;
  Take take;
  Curve curve;
  Vector<Turn> turns;
  Vector<Choice> choices;
};

}  // namespace SOUND
