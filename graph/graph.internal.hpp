// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "graph.hpp"
#include "graph.plan.hpp"
#include "graph.retire.hpp"
#include "graph.vetted.hpp"

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
auto stand(Whole at) -> Stand &;

auto slot(const Stand &stand) -> Stand &;

void retire(Stand &stand);
void retire(Whole at);

void reap();

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

auto circled(const Wire &candidate) -> Flag;

void rebind();

auto surfaced(const Node &node) -> Flag;

void shed(const Wire &drawn);
void shed(const String &gone);

void unprobed();

}  // namespace SOUND::GRAPH
