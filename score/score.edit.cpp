// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <iterator>

#include "score.doors.hpp"

namespace {

auto covered(const SOUND::Note &one, const SOUND::Note &other) -> Flag {
  return one.pitch == other.pitch && one.at < other.at + other.length &&
         other.at < one.at + one.length;
}

auto held(const SOUND::Score &score, const Vector<SOUND::SCORE::Landing> &set)
  -> Flag {
  for (Whole one = 0; one < set.size(); ++one) {
    if (set[one].index >= score.notes.size()) return false;
    for (Whole other = one + 1; other < set.size(); ++other)
      if (set[other].index == set[one].index) return false;
  }
  return !set.empty();
}

auto lifted(SOUND::Score &score, const Vector<SOUND::SCORE::Landing> &set)
  -> Vector<SOUND::Note> {
  Vector<SOUND::Note> carried;
  Vector<Whole> order;
  for (const SOUND::SCORE::Landing &fall : set) {
    SOUND::Note note = score.notes[fall.index];
    note.at = fall.at;
    note.pitch = fall.pitch;
    carried.push_back(note);
    order.push_back(fall.index);
  }
  std::sort(order.begin(), order.end());
  for (Whole taken = order.size(); taken > 0; --taken)
    score.notes.erase(
      std::next(score.notes.begin(), Integer(order[taken - 1])));
  return carried;
}

auto kept(const Vector<SOUND::Note> &carried) -> Vector<Flag> {
  Vector<Flag> keeps(carried.size(), true);
  for (Whole one = 0; one < carried.size(); ++one)
    for (Whole later = one + 1; later < carried.size(); ++later)
      if (::covered(carried[later], carried[one])) keeps[one] = false;
  return keeps;
}

}  // namespace

auto SOUND::SCORE::move(Score &score, Whole index, Whole at, Whole pitch)
  -> Whole {
  if (index >= score.notes.size()) return NONE;
  const Note carried = {
    .pitch = pitch,
    .at = at,
    .length = score.notes[index].length,
    .velocity = score.notes[index].velocity};
  score.notes.erase(std::next(score.notes.begin(), Integer(index)));
  return write(score, carried);
}

auto SOUND::SCORE::land(Score &score, Whole index, Whole at, Whole pitch)
  -> Whole {
  const Vector<Whole> landed = land(score, {{index, at, pitch}});
  return landed.empty() ? NONE : landed.front();
}

auto SOUND::SCORE::land(Score &score, const Vector<Landing> &set)
  -> Vector<Whole> {
  if (!::held(score, set)) return {};
  const Vector<Note> carried = ::lifted(score, set);
  const Vector<Flag> keeps = ::kept(carried);
  for (Whole one = 0; one < carried.size(); ++one) {
    if (!keeps[one]) continue;
    for (auto other = score.notes.begin(); other != score.notes.end();)
      other =
        ::covered(carried[one], *other) ? score.notes.erase(other) : other + 1;
  }
  Vector<Whole> landed(set.size(), NONE);
  for (Whole one = 0; one < carried.size(); ++one) {
    if (!keeps[one]) continue;
    const Whole at = write(score, carried[one]);
    for (Whole prior = 0; prior < one; ++prior)
      if (landed[prior] != NONE && landed[prior] >= at) ++landed[prior];
    landed[one] = at;
  }
  return landed;
}

auto SOUND::SCORE::stretch(Score &score, Whole index, Whole length) -> Flag {
  if (index >= score.notes.size() || length == 0) return false;
  score.notes[index].length = length;
  return true;
}

auto SOUND::SCORE::erase(Score &score, Whole index) -> Flag {
  if (index >= score.notes.size()) return false;
  score.notes.erase(std::next(score.notes.begin(), Integer(index)));
  return true;
}
