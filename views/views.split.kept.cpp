// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../graph.hpp"
#include "views.internal.hpp"
#include "views.kept.hpp"
#include "views.split.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot HEIGHT = "split.height";
constexpr STRING::Hot LAST = "split.last";
constexpr STRING::Hot VIEW = "split.view";


}  // namespace

void SOUND::VIEWS::SPLIT::keep(FIELDS::Map &face) {
  face[::HEIGHT] = height();
  face[::LAST] = last();
  const Whole at = lower();
  if (at != NONE) face[::VIEW] = String(roster()[at].name);
}

void SOUND::VIEWS::SPLIT::restore(const FIELDS::Map &face) {
  Float tall = height();
  KEPT::number(face, ::HEIGHT, tall);
  Float kept = tall;
  KEPT::number(face, ::LAST, kept);
  restore(tall, kept);
  String name;
  if (KEPT::text(face, ::VIEW, name)) choose(name);
}
