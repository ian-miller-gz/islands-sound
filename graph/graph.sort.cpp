// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "graph.internal.hpp"

namespace {

auto sooner(const AUDIO::PLUGIN::Event &one, const AUDIO::PLUGIN::Event &two)
  -> Flag {
  return one.offset < two.offset;
}

}  // namespace

void SOUND::GRAPH::ordered(
  Vector<AUDIO::PLUGIN::Event> &run, const AUDIO::PLUGIN::Event &edge) {
  Whole place = run.size();
  while (place > 0 && edge.offset < run[place - 1].offset) --place;
  run.insert(run.begin() + static_cast<Integer>(place), edge);
}

void SOUND::GRAPH::sort(Stand &stand) {
  for (Run &run : written(stand).runs)
    std::stable_sort(run.developed.begin(), run.developed.end(), ::sooner);
}

void SOUND::GRAPH::sort() {
  for (Stand &stand : stands()) sort(stand);
}

void SOUND::GRAPH::sort(const Vector<String> &nodes) {
  for (const String &node : nodes)
    if (const Whole stood = at(node); stood != NONE) sort(stands()[stood]);
}
