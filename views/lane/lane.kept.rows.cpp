// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../views.kept.hpp"
#include "lane.internal.hpp"

namespace {
using namespace SOUND;
namespace LANE = SOUND::VIEWS::LANE;
namespace KEPT = SOUND::VIEWS::KEPT;

constexpr STRING::Hot MARKED = "lane.mark";
constexpr STRING::Hot FACES = "lane.faces";

struct Marking {
  STRING::Hot leaf = "";
  Whole LANE::Mark::*field = nullptr;
};

constexpr Marking MARKS[] = {
  {"track", &LANE::Mark::track},
  {"lane", &LANE::Mark::lane},
  {"kind", &LANE::Mark::kind}};

auto marking(STRING::Hot leaf) -> String {
  return String(::MARKED) + "." + leaf;
}

}  // namespace

void SOUND::VIEWS::LANE::keep(FIELDS::Map &face) {
  record();
  for (const ::Marking &row : ::MARKS)
    face[::marking(row.leaf)] = stored().*row.field;
  face[::FACES] = records().size();
  Whole n = 0;
  for (Face &one : records()) keep(face, n++, one);
}

void SOUND::VIEWS::LANE::restore(const FIELDS::Map &face) {
  for (const ::Marking &row : ::MARKS)
    KEPT::number(face, ::marking(row.leaf), stored().*row.field);
  Whole count = 0;
  if (!KEPT::number(face, ::FACES, count)) return;
  records().clear();
  forget();
  for (Whole n = 0; n < count; ++n) restore(face, n, records().emplace_back());
}
