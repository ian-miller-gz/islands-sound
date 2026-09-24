// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../inventory.hpp"
#include "../../timeline.hpp"
#include "../../transport.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;

struct Notches {
  Whole first = 0;
  Whole stride = 0;
};

auto turned(const Box &box) -> Notches {
  const Inventory &pool = INVENTORY::held();
  if (box.stock >= pool.stocks.size()) return {};
  const Stock &stock = pool.stocks[box.stock];
  const Whole carried = INVENTORY::length(stock);
  if (carried == 0) return {};
  const Whole opens = TIMELINE::opened(0, box.from, carried);
  const Whole first = opens == 0 ? carried : opens;
  if (stock.kind != KIND::NOTES) return {first, carried};
  const Vector<Tempo> &tempos = TRANSPORT::held().tempos;
  return {SCORE::framed(first, tempos), SCORE::framed(carried, tempos)};
}

auto counted(const Notches &marks, Whole frames) -> Whole {
  if (marks.stride == 0 || marks.first >= frames) return 0;
  return 1 + (frames - marks.first - 1) / marks.stride;
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::stocked(const Box &box, Whole frames) -> Whole {
  return box.kind == KIND::NOTES
           ? SCORE::pulsed(frames, TRANSPORT::held().tempos)
           : frames;
}

auto SOUND::VIEWS::ARRANGEMENT::marks(const Box &box) -> Vector<Whole> {
  const Notches turns = ::turned(box);
  const Whole wanted = ::counted(turns, box.frames);
  Vector<Whole> at;
  for (Whole mark = 0; mark < wanted; ++mark)
    at.push_back(turns.first + mark * turns.stride);
  return at;
}

void SOUND::VIEWS::ARRANGEMENT::notches(SHELL::Session &session) {
  const Vector<Box> &laid = boxes();
  for (Whole row = 0; row < laid.size(); ++row) {
    const Vector<Whole> at = marks(laid[row]);
    String said;
    for (const Whole mark : at) said += std::format(" {}", mark);
    session.print(std::format("notch {} count {}{}", row, at.size(), said));
  }
}
