// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "graph.hpp"

namespace SOUND::GRAPH {

struct Run {
  Vector<AUDIO::PLUGIN::Event> developed;
  Vector<Vector<Float>> laid;
};

struct Tape {
  Vector<Run> runs;
  Vector<AUDIO::PLUGIN::Event> dialled;
  Whole stamp = 0;
};

struct Stand {
  const PLUGIN::Offer *offer = nullptr;
  void *instance = nullptr;
  Tape tapes[2];
  Intake held;
  Intake intakes[2];
  Whole stirred = 1;
};

auto standing() -> Graph &;
auto stands() -> Vector<Stand> &;

auto added(const Node &node, const Stand &stand) -> String;

void stir();

void reckon();

auto written(Stand &stand) -> Tape &;
auto walked(const Stand &stand) -> const Tape &;

void sort(Stand &stand);

void ordered(
  Vector<AUDIO::PLUGIN::Event> &run, const AUDIO::PLUGIN::Event &edge);

auto stamped(const String &word) -> String;

auto mirrored(const Port &port) -> Port;

auto worn(const String &node, Whole kind) -> Whole;

auto noted(const String &node, Whole out) -> Flag;

auto vetted(const String &node, Whole parameter) -> STRING::Hot;

auto circled(const Wire &candidate) -> Flag;

void rebind();

auto surfaced(const Node &node) -> Flag;

void shed(const Wire &drawn);
void shed(const String &gone);

void unprobed();

}  // namespace SOUND::GRAPH
