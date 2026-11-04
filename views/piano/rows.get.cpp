// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.internal.hpp"
#include "piano.rows.get.hpp"

auto SOUND::VIEWS::ROLL::ROWS::lane(Whole pitch, Whole at) -> Float {
  return GET::ladder().lane(pitch, at);
}

auto SOUND::VIEWS::ROLL::ROWS::pitch(Float y, Whole at) -> Whole {
  return GET::ladder().pitch(y, at);
}

auto SOUND::VIEWS::ROLL::ROWS::deep() -> Float { return GET::ladder().deep(); }
