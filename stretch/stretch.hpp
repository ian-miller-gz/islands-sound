// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../stock.hpp"

namespace SOUND::STRETCH {

constexpr Whole WINDOW = 2048;
constexpr Whole SEEK = 512;
constexpr Whole JOIN = 480;
constexpr Float OCTAVE = 1200.0f;

struct Span {
  Whole at = 0, frames = 0;
  Integer cents = 0;
};

auto stretched(const Take &take, Float ratio, Integer cents) -> Take;

auto spliced(const Take &take, const Vector<Span> &spans) -> Take;

}  // namespace SOUND::STRETCH
