// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../control.hpp"
#include "graph.browse.internal.hpp"
#include "graph.keys.hpp"
#include "graph.replace.hpp"

namespace {
using namespace SOUND;
using namespace SOUND::VIEWS::WIRED::BROWSE;
namespace WIRED = SOUND::VIEWS::WIRED;
}  // namespace

void SOUND::VIEWS::WIRED::BROWSE::took(
  GUI::Handle page, const Vector<Offer> &rows) {
  const Whole row = GUI::GET::cursor(page, ROWS);
  if (row >= rows.size()) return;
  const Offer &taken = rows[row];
  if (taken.directory) return enter(taken.among);
  const String replaced = WIRED::replacing();
  WIRED::raise(false);
  if (taken.from == FILES) return void(land(taken.path));
  if (devised(taken.from))
    return WIRED::surface(
      surfacing(taken.from), taken.device, taken.lane == NONE ? 0 : taken.lane);
  if (!taken.target.empty()) return WIRED::send(taken.target);
  if (taken.from == WIRED::INPUTS)
    return taken.lane == NONE ? CONTROL::choose(taken.name)
                              : CONTROL::choose(taken.lane);
  if (!replaced.empty()) return void(WIRED::replace(replaced, taken.name));
  WIRED::take(taken.name, ROWS, row);
}
