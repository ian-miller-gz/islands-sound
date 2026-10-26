// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <cmath>
#include <format>

#include "../graph.hpp"
#include "../render.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;
}  // namespace

void SOUND::COMMANDS::peak(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("peak <node>");
  const String &node = session.arguments[1];
  if (GRAPH::at(node) == NONE)
    return session.print(std::format("peak refused: no node {}", node));
  Float loudest = 0.0f;
  for (const Vector<AUDIO::PLUGIN::Sample> &lane : RENDER::sounded(node))
    for (const AUDIO::PLUGIN::Sample sample : lane)
      loudest = std::max(loudest, std::abs(sample));
  session.print(std::format("peak {} {:.4f}", node, loudest));
}
