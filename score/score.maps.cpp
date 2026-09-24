// SPDX-License-Identifier: AGPL-3.0-or-later
#include "score.doors.hpp"

namespace {
using namespace SOUND;

template <typename Row>
auto standing(const Vector<Row> &rows, Whole at) -> Row {
  if (rows.empty()) return Row{};
  Whole seat = 0;
  while (seat + 1 < rows.size() && rows[seat + 1].at <= at) ++seat;
  return rows[seat];
}

}  // namespace

auto SOUND::SCORE::paced(const Vector<Tempo> &tempos) -> Float {
  return paced(tempos, 0);
}

auto SOUND::SCORE::paced(const Vector<Tempo> &tempos, Whole at) -> Float {
  return tempos.empty() ? TEMPO : ::standing(tempos, at).tempo;
}

auto SOUND::SCORE::metred(const Vector<Metre> &metres) -> Metre {
  return metred(metres, 0);
}

auto SOUND::SCORE::metred(const Vector<Metre> &metres, Whole at) -> Metre {
  return ::standing(metres, at);
}

auto SOUND::SCORE::keyed(const Vector<Key> &keys) -> Key {
  return keyed(keys, 0);
}

auto SOUND::SCORE::keyed(const Vector<Key> &keys, Whole at) -> Key {
  return ::standing(keys, at);
}

auto SOUND::SCORE::framed(Whole pulses, const Vector<Tempo> &tempos) -> Whole {
  Whole frames = 0, at = 0;
  Float tempo = paced(tempos);
  for (const Tempo &row : tempos) {
    if (row.at >= pulses) break;
    if (row.at > at) {
      frames += framed(row.at - at, tempo);
      at = row.at;
    }
    tempo = row.tempo;
  }
  return frames + framed(pulses - at, tempo);
}

auto SOUND::SCORE::pulsed(Whole frames, const Vector<Tempo> &tempos) -> Whole {
  Whole at = 0, run = 0;
  Float tempo = paced(tempos);
  for (const Tempo &row : tempos) {
    if (row.at <= at) {
      tempo = row.tempo;
      continue;
    }
    const Whole span = framed(row.at - at, tempo);
    if (run + span >= frames) break;
    run += span;
    at = row.at;
    tempo = row.tempo;
  }
  return at + pulsed(frames - run, tempo);
}
