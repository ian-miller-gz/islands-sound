// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "../stock.hpp"

namespace SOUND::RENDER::REST {

struct Spanned {
  Address address;
  Whole opens = 0, closes = 0;
};

auto spanned() -> Vector<Spanned>;

auto covered(const Vector<Spanned> &spans, const Address &address, Whole at)
  -> Flag;

}  // namespace SOUND::RENDER::REST
