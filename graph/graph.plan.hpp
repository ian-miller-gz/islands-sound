// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "graph.hpp"

namespace SOUND::GRAPH {

struct Stand;

struct Plan {
  Vector<Node> nodes;
  Vector<Wire> wires;
  Vector<Stand *> stands;
  Vector<Whole> order;
  Flag whole = false;
  Whole count = 0;
};

auto played() -> const Plan &;

void plan(const Vector<Whole> &order, Flag whole);

namespace GET {

auto seated() -> const Vector<Stand *> &;

}  // namespace GET

namespace PLAN {

constexpr Whole SIDES = 2;

void owe();

auto spare() -> Whole;

void adopt();

auto adopted() -> Whole;

auto written() -> Whole;

auto side() -> Whole;

auto at(const String &name) -> Whole;

auto offered(Whole node) -> const PLUGIN::Offer *;

auto instance(Whole node) -> void *;

auto developed(Whole node, Whole out) -> const Vector<AUDIO::PLUGIN::Event> &;

auto laid(Whole node, Whole out) -> const Vector<Vector<Float>> &;

auto dialled(Whole node) -> const Vector<AUDIO::PLUGIN::Event> &;

auto taken(Whole node) -> const Intake &;

}  // namespace PLAN

}  // namespace SOUND::GRAPH
