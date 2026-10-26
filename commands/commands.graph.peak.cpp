// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <charconv>
#include <cmath>
#include <format>

#include "../graph.hpp"
#include "../render.hpp"
#include "../commands.hpp"

namespace {
using namespace SOUND;

using Lane = Vector<AUDIO::PLUGIN::Sample>;

auto loudest(const Lane &lane) -> Float {
  Float most = 0.0f;
  for (const AUDIO::PLUGIN::Sample sample : lane)
    most = std::max(most, std::abs(sample));
  return most;
}

void lone(SHELL::Session &session, const String &node, const String &word) {
  const Vector<Lane> &lanes = RENDER::sounded(node);
  Whole lane = NONE;
  const auto [end, fault] =
    std::from_chars(word.data(), word.data() + word.size(), lane);
  if (
    fault != std::errc{} || end != word.data() + word.size() ||
    lane >= lanes.size())
    return session.print(std::format("peak refused: no lane {}", word));
  session.print(
    std::format("peak {} lane {} {:.4f}", node, lane, loudest(lanes[lane])));
}

}  // namespace

void SOUND::COMMANDS::peak(SHELL::Session &session) {
  if (session.arguments.size() < 2) return session.print("peak <node> [lane]");
  const String &node = session.arguments[1];
  if (GRAPH::at(node) == NONE)
    return session.print(std::format("peak refused: no node {}", node));
  if (session.arguments.size() > 2)
    return ::lone(session, node, session.arguments[2]);
  Float most = 0.0f;
  for (const Lane &lane : RENDER::sounded(node))
    most = std::max(most, ::loudest(lane));
  session.print(std::format("peak {} {:.4f}", node, most));
}
