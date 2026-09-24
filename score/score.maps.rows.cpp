// SPDX-License-Identifier: AGPL-3.0-or-later
#include "score.doors.hpp"

namespace {
using namespace SOUND;

template <typename Row>
auto seated(Vector<Row> &rows, const Row &row) -> Whole {
  Whole seat = 0;
  while (seat < rows.size() && rows[seat].at < row.at) ++seat;
  if (seat < rows.size() && rows[seat].at == row.at) {
    rows[seat] = row;
    return seat;
  }
  rows.insert(rows.begin() + Integer(seat), row);
  return seat;
}

template <typename Row>
auto lifted(Vector<Row> &rows, Whole at) -> Flag {
  if (at == 0) return false;
  for (Whole seat = 0; seat < rows.size(); ++seat)
    if (rows[seat].at == at) {
      rows.erase(rows.begin() + Integer(seat));
      return true;
    }
  return false;
}

}  // namespace

auto SOUND::SCORE::placed(Vector<Tempo> &tempos, const Tempo &row) -> Whole {
  return ::seated(tempos, row);
}

auto SOUND::SCORE::placed(Vector<Metre> &metres, const Metre &row) -> Whole {
  return ::seated(metres, row);
}

auto SOUND::SCORE::placed(Vector<Key> &keys, const Key &row) -> Whole {
  return ::seated(keys, row);
}

auto SOUND::SCORE::removed(Vector<Tempo> &tempos, Whole at) -> Flag {
  return ::lifted(tempos, at);
}

auto SOUND::SCORE::removed(Vector<Metre> &metres, Whole at) -> Flag {
  return ::lifted(metres, at);
}

auto SOUND::SCORE::removed(Vector<Key> &keys, Whole at) -> Flag {
  return ::lifted(keys, at);
}
