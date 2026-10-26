// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include <common/types.hpp>

namespace SOUND {
struct Node;
}  // namespace SOUND

namespace SOUND::RENDER {

void plan();

auto kept(const Node &node) -> Whole;

}  // namespace SOUND::RENDER
