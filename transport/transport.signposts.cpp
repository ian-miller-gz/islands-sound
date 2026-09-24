// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "../timeline.hpp"
#include "transport.internal.hpp"

namespace {
using namespace SOUND;

auto framed(Whole pulses) -> Whole {
  return SCORE::framed(pulses, TRANSPORT::clockwork().setting.tempos);
}

auto posts() -> Vector<Whole> {
  const Setting &setting = TRANSPORT::clockwork().setting;
  Vector<Whole> places;
  for (const Tag &tag : setting.tags) places.push_back(::framed(tag.at));
  if (setting.ends.to > setting.ends.from) {
    places.push_back(::framed(setting.ends.from));
    places.push_back(::framed(setting.ends.to));
  }
  return places;
}

auto last() -> Whole {
  const Setting &setting = TRANSPORT::clockwork().setting;
  return std::max(::framed(setting.ends.to), TIMELINE::ended());
}

}  // namespace

auto SOUND::TRANSPORT::tag(Whole at, const String &text) -> Whole {
  Vector<Tag> &tags = clockwork().setting.tags;
  Whole seat = 0;
  while (seat < tags.size() && tags[seat].at < at) ++seat;
  if (seat < tags.size() && tags[seat].at == at) {
    tags[seat].text = text;
    return seat;
  }
  tags.insert(tags.begin() + Integer(seat), Tag{at, text});
  return seat;
}

auto SOUND::TRANSPORT::untag(Whole index) -> Flag {
  Vector<Tag> &tags = clockwork().setting.tags;
  if (index >= tags.size()) return false;
  tags.erase(tags.begin() + Integer(index));
  return true;
}

auto SOUND::TRANSPORT::loop(Whole from, Whole to) -> Whole {
  if (to <= from) return NONE;
  Vector<Span> &loops = clockwork().setting.loops;
  Whole seat = 0;
  while (seat < loops.size() && loops[seat].from <= from) ++seat;
  loops.insert(loops.begin() + Integer(seat), Span{from, to});
  state();
  return seat;
}

auto SOUND::TRANSPORT::unloop(Whole index) -> Flag {
  Vector<Span> &loops = clockwork().setting.loops;
  if (index >= loops.size()) return false;
  loops.erase(loops.begin() + Integer(index));
  state();
  return true;
}

auto SOUND::TRANSPORT::loop(Whole index, const String &text) -> Flag {
  Vector<Span> &loops = clockwork().setting.loops;
  if (index >= loops.size()) return false;
  loops[index].text = text;
  return true;
}

void SOUND::TRANSPORT::loop(Flag looping) {
  clockwork().setting.looping = looping;
  state();
}

void SOUND::TRANSPORT::ends(Whole from, Whole to) {
  clockwork().setting.ends = to > from ? Span{from, to} : Span{};
}

auto SOUND::TRANSPORT::ahead() -> Whole {
  const Whole at = marker().position, end = ::last();
  Whole next = at;
  for (Whole place : ::posts())
    if (place > at && (next == at || place < next)) next = place;
  if (end > at && (next == at || end < next)) next = end;
  if (next != at) locate(next);
  return next;
}

auto SOUND::TRANSPORT::behind() -> Whole {
  const Whole at = marker().position;
  Whole last = 0;
  for (Whole place : ::posts())
    if (place < at && place > last) last = place;
  if (last != at) locate(last);
  return last;
}
