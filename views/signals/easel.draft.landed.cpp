// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../../inventory.hpp"
#include "easel.draft.hpp"

namespace {
using namespace SOUND;
namespace DRAFT = SOUND::VIEWS::SIGNALS::DRAFT;

constexpr STRING::Hot DREW = "draw";

void cleared() {
  const Vector<VIEWS::SIGNALS::Point> &drafting = DRAFT::points();
  if (!DRAFT::spanning() || drafting.size() < 2) return;
  const VIEWS::SIGNALS::Aim aim = VIEWS::SIGNALS::aimed(drafting.front().at);
  const VIEWS::SIGNALS::Aim far = VIEWS::SIGNALS::aimed(drafting.back().at);
  if (aim.stock == NONE || far.stock != aim.stock) return;
  const Whole opens = VIEWS::SIGNALS::inside(aim, drafting.front().at);
  const Whole closes = VIEWS::SIGNALS::inside(far, drafting.back().at);
  HISTORY::take(aim.stock, INVENTORY::held().stocks[aim.stock]);
  VIEWS::SIGNALS::clear(
    aim.stock, std::min(opens, closes), std::max(opens, closes));
}

}  // namespace

void SOUND::VIEWS::SIGNALS::spanned(
  const GUI::SAC::STROKE::Cell &from, const GUI::SAC::STROKE::Cell &to,
  Whole shape) {
  Vector<Point> &drafting = DRAFT::points();
  drafting.clear();
  DRAFT::spanning() = true;
  DRAFT::lay(from.across, valued(from.down), shape);
  if (stamped(to.across) == stamped(from.across)) return;
  DRAFT::lay(to.across, valued(to.down), DRAFT::standing(stamped(to.across)));
  if (drafting.size() == 2) drafting.front().until = drafting.back().at;
}

auto SOUND::VIEWS::SIGNALS::landed() -> Whole {
  Vector<Point> &drafting = DRAFT::points();
  Whole laid = 0;
  if (!DRAFT::spanning()) {
    thinned(drafting);
    DRAFT::spanning() = drafting.size() > 1;
  }
  HISTORY::begin();
  ::cleared();
  for (const Point &point : drafting) {
    const Aim aim = aimed(point.at);
    if (aim.stock == NONE) continue;
    const Row laying = {inside(aim, point.at), point.value, point.shape};
    const Whole held = covered(aim.stock, laying.at);
    HISTORY::take(aim.stock, INVENTORY::held().stocks[aim.stock]);
    if (held != NONE) INVENTORY::erase(aim.stock, held);
    if (write(aim.stock, laying)) ++laid;
    HISTORY::wrote(::DREW, aim.stock, INVENTORY::held().stocks[aim.stock]);
  }
  begun();
  HISTORY::end();
  return laid;
}
