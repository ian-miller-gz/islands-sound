// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../graph.hpp"
#include "render.hpp"
#include "render.voice.hpp"
#include "render.words.hpp"

namespace {
using namespace SOUND;

void count(SHELL::Session &session) {
  const RENDER::VOICE::Blocks blocks = RENDER::VOICE::blocks();
  session.print(std::format(
    "blocks {} torn {} short {} retired {}", blocks.ended, blocks.torn,
    blocks.underruns, GRAPH::retired()));
}

}  // namespace

auto SOUND::RENDER::words() -> Vector<SHELL::Command> {
  return {{"blocks", "", ::count}};
}
