// SPDX-License-Identifier: AGPL-3.0-or-later
#include <island/gui/seat.hpp>

#include "../boards.hpp"
#include "views.ends.hpp"

namespace {
using namespace SOUND;
using VIEWS::ENDS::Bed;
using VIEWS::ENDS::Pool;

constexpr STRING::Hot GROUND = "chrome";
constexpr STRING::Hot INKED = "endline";
constexpr STRING::Hot HELD = "endhead";
constexpr Float UNWORN = -1.0f;
constexpr Float LAID = 0.5f;
constexpr Float DOTTED = 1.5f;  // the run's dash, in board units

void stand(GUI::Handle page, const Bed &bed, Whole row) {
  const String id = VIEWS::ENDS::named(bed, row);
  BOARDS::place(page, VIEWS::ENDS::bedded(bed).c_str(), "panel", id, {}, {});
  GUI::set(page, id.c_str(), GUI::Style{::GROUND});
  GUI::set(page, id.c_str(), GUI::Depth{::LAID});
  const String run = id + "." + VIEWS::ENDS::RUN;
  BOARDS::place(page, id.c_str(), "stroke", run, {}, {});
  GUI::set(page, run.c_str(), GUI::Style{::INKED});
  GUI::set(page, run.c_str(), GUI::Dash{::DOTTED});
  const String head = id + "." + VIEWS::ENDS::HEAD;
  BOARDS::place(page, id.c_str(), "grip", head, {}, {});
  GUI::set(page, head.c_str(), GUI::Style{::HELD});
  GUI::set(page, head.c_str(), GUI::Axis{.across = true});
}

}  // namespace

auto SOUND::VIEWS::ENDS::bedded(const Bed &bed) -> String {
  return bed.board + "." + BED;
}

auto SOUND::VIEWS::ENDS::headed(const Bed &bed) -> Float {
  return bed.scale > 0.0f ? HEADED / bed.scale : HEADED;
}

auto SOUND::VIEWS::ENDS::named(const Bed &bed, Whole row) -> String {
  return GUI::SAC::SEAT::named(bedded(bed).c_str(), row);
}

void SOUND::VIEWS::ENDS::build(
  GUI::Handle page, const Bed &bed, Whole count, Pool &pool) {
  const String ground = bedded(bed);
  if (pool.stood == 0 && count != 0)
    BOARDS::place(page, bed.board.c_str(), "panel", ground, {}, {});
  BOARDS::sweep(page, ground.c_str(), count, pool.stood);
  for (Whole row = pool.stood; row < count; ++row) ::stand(page, bed, row);
  pool.stood = count;
  pool.worn.resize(count, ::UNWORN);
}

void SOUND::VIEWS::ENDS::bare(GUI::Handle page, const Bed &bed, Pool &pool) {
  if (pool.stood == 0) return;
  build(page, bed, 0, pool);
  BOARDS::drop(page, bedded(bed));
}
