// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "shape.hpp"
#include "../score.hpp"

namespace SOUND::SHAPE {

auto spoken(Whole shape) -> STRING::Hot;

auto meant(const String &token) -> Whole;

auto reading(Whole shape, Float part, Float bend) -> Float;

template <typename Node>
auto reading(const Node &node, Float part) -> Float {
  return reading(node.shape, part, node.bend);
}

}  // namespace SOUND::SHAPE
