// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common.hpp>

namespace SOUND::INVENTORY::RIFF {

struct Shape {
  Whole format = 0;
  Whole channels = 0;
  Whole rate = 0;
  Whole width = 0;
  Whole at = 0;
  Whole size = 0;
};

auto survey(const Vector<Byte> &bytes, Shape &shape) -> Flag;

auto sampled(const Vector<Byte> &bytes, Whole at) -> Integer;

}  // namespace SOUND::INVENTORY::RIFF
