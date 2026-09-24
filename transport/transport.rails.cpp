// SPDX-License-Identifier: AGPL-3.0-or-later
#include "transport.internal.hpp"

namespace {
using std::memory_order_acquire;
using std::memory_order_relaxed;
using std::memory_order_release;
}  // namespace

void SOUND::TRANSPORT::Mirror::publish(const Reading &reading) {
  const Whole live = face.load(memory_order_relaxed);
  pair[1 - live] = reading;
  face.store(1 - live, memory_order_release);
}

auto SOUND::TRANSPORT::Mirror::read() -> Reading {
  return pair[face.load(memory_order_acquire)];
}

void SOUND::TRANSPORT::Deck::publish(const Statement &want) {
  const Whole live = face.load(memory_order_relaxed);
  pair[1 - live] = want;
  face.store(1 - live, memory_order_release);
}

auto SOUND::TRANSPORT::Deck::read() -> Statement {
  return pair[face.load(memory_order_acquire)];
}

void SOUND::TRANSPORT::detach() {
  Clockwork &works = clockwork();
  if (works.locates != works.want.locates) return;
  works.want.mark = works.position;
}

void SOUND::TRANSPORT::advance(Whole frames) {
  Clockwork &works = clockwork();
  const Statement want = works.deck.read();
  if (want.locates != works.locates) {
    works.locates = want.locates;
    works.position = want.mark;
  }
  works.taken = want;
  works.started = works.position;
  if (want.playing) works.position += frames;
  const Whole span = want.to > want.from ? want.to - want.from : 0;
  if (want.looping && span != 0 && works.position >= want.to)
    works.position = want.from + (works.position - want.to) % span;
  works.mirror.publish({works.position, works.locates});
}

auto SOUND::TRANSPORT::block() -> const Statement & {
  return clockwork().taken;
}

auto SOUND::TRANSPORT::started() -> Whole { return clockwork().started; }
