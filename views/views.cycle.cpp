// SPDX-License-Identifier: AGPL-3.0-or-later
#include "../commands.hpp"
#include "../history.hpp"
#include "../transport.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

auto apart(Whole one, Whole two) -> Whole {
  return one > two ? one - two : two - one;
}

void dropped(Whole from, Whole to) {
  if (!COMMANDS::looped(VIEWS::pulsed(from), VIEWS::pulsed(to), String()))
    return;
  TRANSPORT::loop(true);
}

auto nearest(Whole head, Whole tail) -> Whole {
  const Vector<Span> &loops = TRANSPORT::held().loops;
  Whole near = NONE, best = 0;
  for (Whole at = 0; at < loops.size(); ++at) {
    const Whole from = VIEWS::framed(loops[at].from);
    const Whole to = VIEWS::framed(loops[at].to);
    const Whole gap = head >= to ? head - to : (tail <= from ? from - tail : 0);
    if (near == NONE || gap < best) near = at, best = gap;
  }
  return near;
}

void stretched(Whole near, Whole head, Whole tail) {
  const Span loop{
    VIEWS::framed(TRANSPORT::held().loops[near].from),
    VIEWS::framed(TRANSPORT::held().loops[near].to)};
  HISTORY::begin();
  COMMANDS::looped(TRANSPORT::held().loops[near].from);
  if (head >= loop.to)
    ::dropped(loop.from, tail);
  else if (tail <= loop.from)
    ::dropped(head, loop.to);
  else if (::apart(head, loop.from) <= ::apart(tail, loop.to))
    ::dropped(head, loop.to);
  else
    ::dropped(loop.from, tail);
  HISTORY::end();
}

}  // namespace

auto SOUND::VIEWS::framed(Whole pulses) -> Whole {
  return SCORE::framed(pulses, TRANSPORT::held().tempos);
}

auto SOUND::VIEWS::pulsed(Whole frames) -> Whole {
  return SCORE::pulsed(frames, TRANSPORT::held().tempos);
}

void SOUND::VIEWS::cycled(Integer at, Whole span) {
  const Integer end = at + Integer(span);
  if (end <= 0) return;
  const Whole head = at < 0 ? 0 : Whole(at);
  const Whole tail = Whole(end);
  const Whole near = ::nearest(head, tail);
  if (near == NONE) return ::dropped(head, tail);
  ::stretched(near, head, tail);
}

void SOUND::VIEWS::cycled() {
  const Setting &setting = TRANSPORT::held();
  if (!setting.loops.empty()) return TRANSPORT::loop(!setting.looping);
  const Whole span = TRANSPORT::bar();
  if (span == 0) return;
  const Whole mark = TRANSPORT::mark(), head = mark - mark % span;
  ::dropped(head, head + span);
}
