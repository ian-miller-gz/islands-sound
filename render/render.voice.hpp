// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common/types.hpp>

namespace SOUND::RENDER::VOICE {

struct Blocks {
  Whole begun = 0;
  Whole ended = 0;
  Whole torn = 0;
  Whole underruns = 0;
};

auto blocks() -> Blocks;

auto standing() -> Flag;

void claim();

void drop();

}  // namespace SOUND::RENDER::VOICE
