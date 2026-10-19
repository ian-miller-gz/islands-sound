// SPDX-License-Identifier: AGPL-3.0-or-later
#include "piano.heads.hpp"

namespace SOUND::VIEWS::ROLL {

constexpr Drum DRUMS[] = {
  {35, 36, DIAMOND}, {37, 40, DIAMOND}, {41, 41, DIAMOND}, {42, 42, CROSS},
  {43, 43, DIAMOND}, {44, 44, CROSS},   {45, 45, DIAMOND}, {46, 46, CROSS},
  {47, 48, DIAMOND}, {49, 49, CROSS},   {50, 50, DIAMOND}, {51, 51, CROSS},
  {52, 52, CROSS},   {53, 53, CROSS},   {54, 54, DIAMOND}, {55, 55, CROSS},
  {56, 56, DIAMOND}, {57, 57, CROSS},   {58, 58, DIAMOND}, {59, 59, CROSS},
  {60, 81, DIAMOND},
};

auto headed(Whole note) -> STRING::Hot {
  for (const Drum &drum : DRUMS)
    if (note >= drum.from && note <= drum.to) return drum.head;
  return DIAMOND;
}

}  // namespace SOUND::VIEWS::ROLL
