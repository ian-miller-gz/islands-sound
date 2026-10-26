// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../views.kept.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;
namespace LANE = SOUND::VIEWS::LANE;
namespace KEPT = SOUND::VIEWS::KEPT;

constexpr STRING::Hot TONIC = "tonic";
constexpr STRING::Hot SCALE = "scale";
constexpr STRING::Hot PITCH = "pitch";

auto read(const FIELDS::Map &face, const String &stem, Whole m) -> LANE::Added {
  LANE::Added one;
  KEPT::number(face, KEPT::join(stem, m, ::TONIC), one.tonic);
  KEPT::text(face, KEPT::join(stem, m, ::SCALE), one.scale);
  KEPT::number(face, KEPT::join(stem, m, ::PITCH), one.pitch);
  return one;
}

}  // namespace

void SOUND::VIEWS::LANE::keep(
  FIELDS::Map &face, const String &stem, const Vector<Added> &added) {
  face[stem] = added.size();
  for (Whole m = 0; m < added.size(); ++m) {
    face[KEPT::join(stem, m, ::TONIC)] = added[m].tonic;
    face[KEPT::join(stem, m, ::SCALE)] = added[m].scale;
    face[KEPT::join(stem, m, ::PITCH)] = added[m].pitch;
  }
}

void SOUND::VIEWS::LANE::restore(
  const FIELDS::Map &face, const String &stem, Vector<Added> &added) {
  Whole count = 0;
  KEPT::number(face, stem, count);
  for (Whole m = 0; m < count; ++m) added.push_back(::read(face, stem, m));
}
