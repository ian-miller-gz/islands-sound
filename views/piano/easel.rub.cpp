// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../../inventory.hpp"
#include "../../score.hpp"
#include "../../transport.hpp"
#include "piano.easel.hpp"

namespace {
using namespace SOUND;
namespace EASEL = SOUND::VIEWS::ROLL::EASEL;

EASEL::Cell opened;
Flag rubbing = false;

}  // namespace

void SOUND::VIEWS::ROLL::EASEL::open(const Cell &cell) {
  ::rubbing = false;
  ::opened = cell;
  const Whole span = grained();
  const Whole at = Whole(cell.at) * span;
  const Aim aim = aimed(SCORE::framed(at, TRANSPORT::held().tempos));
  if (aim.stock == NONE) return;
  const Score &score = INVENTORY::held().stocks[aim.stock].score;
  ::rubbing = held(score, cell, inside(aim, at), span) != NONE;
}

auto SOUND::VIEWS::ROLL::ended(
  GUI::Handle page, const GUI::SAC::STROKE::Cell &cell, Flag dragged) -> Flag {
  if (!::rubbing || dragged) return false;
  const Whole span = grained();
  const Whole at = Whole(::opened.at) * span;
  const Aim aim = aimed(SCORE::framed(at, TRANSPORT::held().tempos));
  if (aim.stock == NONE) return true;
  Score score = INVENTORY::held().stocks[aim.stock].score;
  const Whole index = EASEL::held(score, ::opened, inside(aim, at), span);
  if (index == NONE) return true;
  const EASEL::Cell to = EASEL::celled(cell);
  const Whole under =
    EASEL::held(score, to, inside(aim, Whole(to.at) * span), span);
  if (under != index) return true;
  SCORE::erase(score, index);
  HISTORY::take(aim.stock, INVENTORY::held().stocks[aim.stock]);
  INVENTORY::score(aim.stock, score);
  HISTORY::wrote(
    EASEL::PAINTED, aim.stock, INVENTORY::held().stocks[aim.stock]);
  EASEL::said(page, "");
  return true;
}
