// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../transport.hpp"
#include "views.flags.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot LOOPED = "cycleflag";
constexpr STRING::Hot POSTED = "tagflag";
constexpr STRING::Hot BOUND = "endflag";
constexpr STRING::Hot SPAN = "span";
constexpr STRING::Hot PLATE = "panel";
constexpr Float ABOVE = 2.0f;
constexpr Flag OVER = true;
constexpr Flag UNDER = false;
constexpr Flag WORDED = true;
constexpr Flag BARE = false;

struct Seat {
  String strip;
  Whole loops = 0, tags = 0;
};

Vector<Seat> seats;
Whole born = 0;

auto numbered(const String &strip, STRING::Hot word, Whole at) -> String {
  return std::format("{}.{}.{}", strip, word, at);
}

auto named(const String &strip, STRING::Hot word, STRING::Hot part) -> String {
  return std::format("{}.{}.{}", strip, word, part);
}

void place(
  const String &parent, const String &id, STRING::Hot kind, STRING::Hot dress) {
  const GUI::Handle page = VIEWS::document();
  GUI::NODES::create(page, parent.c_str(), kind, id.c_str());
  GUI::set(page, id.c_str(), GUI::Style{dress});
  GUI::set(page, id.c_str(), GUI::Depth{::ABOVE});
}

void lay(const String &id, Flag shown, GUI::Position at, GUI::Extent size) {
  const GUI::Handle page = VIEWS::document();
  GUI::set(page, id.c_str(), GUI::Visibility{shown});
  if (id != VIEWS::carried()) GUI::set(page, id.c_str(), at);
  GUI::set(page, id.c_str(), size);
}

auto seated(const String &strip) -> Seat & {
  if (VIEWS::reborn(::born)) ::seats.clear();
  for (Seat &one : ::seats)
    if (one.strip == strip) return one;
  VIEWS::hoist(
    strip, ::named(strip, VIEWS::ENDS, VIEWS::OPENED), ::BOUND, ::BARE);
  VIEWS::hoist(
    strip, ::named(strip, VIEWS::ENDS, VIEWS::CLOSED), ::BOUND, ::BARE);
  ::seats.push_back(Seat{strip});
  return ::seats.back();
}

void loops(Seat &seat, Whole count) {
  const GUI::Handle page = VIEWS::document();
  for (Whole at = count; at < seat.loops; ++at) {
    const String id = ::numbered(seat.strip, VIEWS::CYCLE, at);
    GUI::NODES::remove(page, (id + "." + ::SPAN).c_str());
    VIEWS::strike(id + "." + VIEWS::OPENED);
    VIEWS::strike(id + "." + VIEWS::CLOSED);
    VIEWS::unword(id);
  }
  for (Whole at = seat.loops; at < count; ++at) {
    const String id = ::numbered(seat.strip, VIEWS::CYCLE, at);
    ::place(seat.strip, id + "." + ::SPAN, ::PLATE, VIEWS::CYCLE);
    VIEWS::hoist(seat.strip, id + "." + VIEWS::OPENED, ::LOOPED, ::WORDED);
    VIEWS::hoist(seat.strip, id + "." + VIEWS::CLOSED, ::LOOPED, ::BARE);
    VIEWS::word(id + "." + VIEWS::OPENED, id);
  }
  seat.loops = count;
}

void tags(Seat &seat, Whole count) {
  for (Whole at = count; at < seat.tags; ++at) {
    const String id = ::numbered(seat.strip, VIEWS::TAG, at);
    VIEWS::unword(id);
    VIEWS::strike(id);
  }
  for (Whole at = seat.tags; at < count; ++at) {
    const String id = ::numbered(seat.strip, VIEWS::TAG, at);
    VIEWS::hoist(seat.strip, id, ::POSTED, ::WORDED);
    VIEWS::word(id, id);
  }
  seat.tags = count;
}

void washes(
  const Seat &seat, Float west, Float scale, Float down, VIEWS::Fold fold) {
  const Vector<Span> &loops = TRANSPORT::held().loops;
  const Span force = TRANSPORT::cycled();
  for (Whole at = 0; at < loops.size(); ++at) {
    const Whole opens = VIEWS::framed(loops[at].from);
    const Whole closes = VIEWS::framed(loops[at].to);
    const Float from = fold(Float(opens), west, scale);
    const Float to = fold(Float(closes), west, scale);
    const Flag wraps =
      TRANSPORT::held().looping && opens == force.from && closes == force.to;
    const String id = ::numbered(seat.strip, VIEWS::CYCLE, at);
    ::lay(id + "." + ::SPAN, wraps, {from, down}, {to - from, VIEWS::BAND});
    VIEWS::fly(
      seat.strip, id + "." + VIEWS::OPENED,
      {.place = from, .above = ::UNDER, .worded = ::WORDED});
    VIEWS::fly(
      seat.strip, id + "." + VIEWS::CLOSED,
      {.place = to, .closing = true, .above = ::UNDER});
    VIEWS::worded(id + "." + VIEWS::OPENED, id, loops[at].text);
  }
}

void posts(const Seat &seat, Float west, Float scale, VIEWS::Fold fold) {
  const Vector<Tag> &tags = TRANSPORT::held().tags;
  for (Whole at = 0; at < tags.size(); ++at) {
    const String id = ::numbered(seat.strip, VIEWS::TAG, at);
    const Float place = fold(Float(VIEWS::framed(tags[at].at)), west, scale);
    VIEWS::fly(
      seat.strip, id, {.place = place, .above = ::OVER, .worded = ::WORDED});
    VIEWS::worded(id, id, tags[at].text);
  }
}

void bounds(const String &strip, Float west, Float scale, VIEWS::Fold fold) {
  const Span &ends = TRANSPORT::held().ends;
  const Flag stands = ends.to > ends.from;
  const Float from = fold(Float(VIEWS::framed(ends.from)), west, scale);
  const Float to = fold(Float(VIEWS::framed(ends.to)), west, scale);
  VIEWS::fly(
    strip, ::named(strip, VIEWS::ENDS, VIEWS::OPENED),
    {.place = from, .shown = stands, .above = ::OVER, .wide = VIEWS::ENDED});
  VIEWS::fly(
    strip, ::named(strip, VIEWS::ENDS, VIEWS::CLOSED),
    {.place = to,
     .shown = stands,
     .closing = true,
     .above = ::OVER,
     .wide = VIEWS::ENDED});
}

}  // namespace

void SOUND::VIEWS::flagged(
  const String &strip, Float west, Float scale, Float down, Fold fold) {
  Seat &seat = ::seated(strip);
  const Setting &setting = TRANSPORT::held();
  ::loops(seat, setting.loops.size());
  ::tags(seat, setting.tags.size());
  ::washes(seat, west, scale, down, fold);
  ::posts(seat, west, scale, fold);
  ::bounds(strip, west, scale, fold);
}
