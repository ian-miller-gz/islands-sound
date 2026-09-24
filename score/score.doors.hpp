// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once
#include "score.hpp"

namespace SOUND::SCORE {

auto framed(Whole pulses, Float tempo) -> Whole;

auto pulsed(Whole frames, Float tempo) -> Whole;

auto framed(Whole pulses, const Vector<Tempo> &tempos) -> Whole;
auto pulsed(Whole frames, const Vector<Tempo> &tempos) -> Whole;

auto paced(const Vector<Tempo> &tempos) -> Float;
auto paced(const Vector<Tempo> &tempos, Whole at) -> Float;
auto metred(const Vector<Metre> &metres) -> Metre;
auto metred(const Vector<Metre> &metres, Whole at) -> Metre;
auto keyed(const Vector<Key> &keys) -> Key;
auto keyed(const Vector<Key> &keys, Whole at) -> Key;

auto placed(Vector<Tempo> &tempos, const Tempo &row) -> Whole;
auto placed(Vector<Metre> &metres, const Metre &row) -> Whole;
auto placed(Vector<Key> &keys, const Key &row) -> Whole;

auto removed(Vector<Tempo> &tempos, Whole at) -> Flag;
auto removed(Vector<Metre> &metres, Whole at) -> Flag;
auto removed(Vector<Key> &keys, Whole at) -> Flag;

auto scales() -> Vector<STRING::Hot>;

auto degree(const Key &key, Whole pitch) -> Whole;

auto snapped(const Key &key, Whole pitch) -> Whole;

auto classed(STRING::Hot name) -> Whole;

auto classed(Whole tonic) -> STRING::Hot;

auto write(Score &score, const Note &note) -> Whole;

auto move(Score &score, Whole index, Whole at, Whole pitch) -> Whole;

auto land(Score &score, Whole index, Whole at, Whole pitch) -> Whole;

struct Landing {
  Whole index = NONE, at = 0, pitch = 0;
};

auto land(Score &score, const Vector<Landing> &set) -> Vector<Whole>;

auto stretch(Score &score, Whole index, Whole length) -> Flag;

auto erase(Score &score, Whole index) -> Flag;

}  // namespace SOUND::SCORE
