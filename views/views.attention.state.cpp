// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iterator>

#include "views.attention.hpp"
#include "views.kept.hpp"
#include "views.split.hpp"

namespace {
using namespace SOUND;
namespace ATTENTION = SOUND::VIEWS::ATTENTION;
namespace KEPT = SOUND::VIEWS::KEPT;

constexpr STRING::Hot HELD = "attention.held";
constexpr STRING::Hot SHOWN = "attention.shown";
constexpr STRING::Hot SLOT = "attention.slot";
constexpr STRING::Hot NODE = "node";

struct Row {
  STRING::Hot leaf = "";
  Whole &(*field)(ATTENTION::Slot &) = nullptr;
};

constexpr Row ROWS[] = {
  {"track", [](ATTENTION::Slot &one) -> Whole & { return one.track; }},
  {"marked", [](ATTENTION::Slot &one) -> Whole & { return one.mark.track; }},
  {"lane", [](ATTENTION::Slot &one) -> Whole & { return one.mark.lane; }},
  {"kind", [](ATTENTION::Slot &one) -> Whole & { return one.mark.kind; }},
  {"clip", [](ATTENTION::Slot &one) -> Whole & { return one.clip; }}};

constexpr Whole REGIONS = std::size(VIEWS::SPLIT::REGIONS);

void kept(FIELDS::Map &face, Whole n) {
  ATTENTION::Slot &one = ATTENTION::get(n);
  ATTENTION::attended(one);
  ATTENTION::aimed(one);
  for (const ::Row &row : ::ROWS)
    face[KEPT::join(::SLOT, n, row.leaf)] = row.field(one);
  face[KEPT::join(::SLOT, n, ::NODE)] = one.node;
}

void restored(const FIELDS::Map &face, Whole n) {
  ATTENTION::Slot &one = ATTENTION::get(n);
  for (const ::Row &row : ::ROWS)
    KEPT::number(face, KEPT::join(::SLOT, n, row.leaf), row.field(one));
  KEPT::text(face, KEPT::join(::SLOT, n, ::NODE), one.node);
}

}  // namespace

void SOUND::VIEWS::ATTENTION::keep(FIELDS::Map &face) {
  for (Whole region = 0; region < ::REGIONS; ++region)
    face[KEPT::join(::HELD, region)] = held(region);
  face[::SHOWN] = shown();
  for (Whole n = 0; n < SLOTS; ++n) ::kept(face, n);
}

void SOUND::VIEWS::ATTENTION::restore(const FIELDS::Map &face) {
  for (Whole region = 0; region < ::REGIONS; ++region) {
    Whole slot = held(region);
    if (KEPT::number(face, KEPT::join(::HELD, region), slot))
      hold(region, slot);
  }
  Whole slot = shown();
  if (KEPT::number(face, ::SHOWN, slot)) show(slot);
  for (Whole n = 0; n < SLOTS; ++n) ::restored(face, n);
}
