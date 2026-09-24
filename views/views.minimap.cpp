// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/gui/nga.hpp>

#include "../boards.hpp"
#include "../transport.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot MARKS = "mark";
constexpr STRING::Hot HEAD = "head";
constexpr STRING::Hot TAGGED = "tagflag";
constexpr STRING::Hot LOOPED = "cycle";
constexpr STRING::Hot ENDED = "endflag";
constexpr STRING::Hot MARKED = "selected";
constexpr Float HAIRLINE = 2.0f;
constexpr Float ORIGIN = 0.0f;
constexpr Float RAISED = 1.0f;

struct Seat {
  String map;
  Whole tags = 0, loops = 0;
  Flag stood = false;
};

Vector<Seat> seats;
Whole born = 0;

auto numbered(const String &map, STRING::Hot word, Whole at) -> String {
  return std::format("{}.{}.{}.{}", map, ::MARKS, word, at);
}

auto named(const String &map, STRING::Hot word) -> String {
  return std::format("{}.{}.{}", map, ::MARKS, word);
}

void place(const String &map, const String &id, STRING::Hot dress) {
  const GUI::Handle page = VIEWS::document();
  BOARDS::place(page, map.c_str(), "panel", id, {}, {});
  GUI::set(page, id.c_str(), GUI::Style{dress});
  GUI::set(page, id.c_str(), GUI::Depth{::RAISED});
  GUI::NGA::set(page, id.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::DOWN});
}

auto seated(const String &map) -> Seat & {
  if (VIEWS::reborn(::born)) ::seats.clear();
  for (Seat &one : ::seats)
    if (one.map == map) return one;
  ::seats.push_back(Seat{map});
  return ::seats.back();
}

void counted(
  Seat &seat, STRING::Hot word, STRING::Hot dress, Whole &stood, Whole count) {
  const GUI::Handle page = VIEWS::document();
  for (Whole at = count; at < stood; ++at)
    GUI::NODES::remove(page, ::numbered(seat.map, word, at).c_str());
  for (Whole at = stood; at < count; ++at)
    ::place(seat.map, ::numbered(seat.map, word, at), dress);
  stood = count;
}

void lay(const String &id, Flag shown, Float at, Float run, Float deep) {
  const GUI::Handle page = VIEWS::document();
  GUI::set(page, id.c_str(), GUI::Visibility{shown});
  GUI::set(page, id.c_str(), GUI::Position{at, ::ORIGIN});
  GUI::set(page, id.c_str(), GUI::Extent{run, deep});
}

}  // namespace

void SOUND::VIEWS::marked(const String &map, Placed placed) {
  const GUI::Handle page = document();
  Seat &seat = ::seated(map);
  if (!seat.stood) {
    for (STRING::Hot word : {OPENED, CLOSED})
      ::place(map, ::named(map, word), ::ENDED);
    ::place(map, ::named(map, ::HEAD), ::MARKED);
    seat.stood = true;
  }
  const Setting &setting = TRANSPORT::held();
  ::counted(seat, TAG, ::TAGGED, seat.tags, setting.tags.size());
  ::counted(seat, CYCLE, ::LOOPED, seat.loops, setting.loops.size());
  const Float deep = GUI::GET::measured(page, map.c_str()).h;
  const Float zoom = GUI::NGA::GET::zoom(page, map.c_str()).value;
  const Float hair = zoom > ::ORIGIN ? ::HAIRLINE / zoom : ::HAIRLINE;
  for (Whole at = 0; at < setting.tags.size(); ++at)
    ::lay(
      ::numbered(map, TAG, at), true, placed(framed(setting.tags[at].at)), hair,
      deep);
  for (Whole at = 0; at < setting.loops.size(); ++at) {
    const Float from = placed(framed(setting.loops[at].from));
    ::lay(
      ::numbered(map, CYCLE, at), true, from,
      placed(framed(setting.loops[at].to)) - from, deep);
  }
  const Flag ended = setting.ends.to > setting.ends.from;
  ::lay(
    ::named(map, OPENED), ended, placed(framed(setting.ends.from)), hair, deep);
  ::lay(
    ::named(map, CLOSED), ended, placed(framed(setting.ends.to)) - hair, hair,
    deep);
  ::lay(::named(map, ::HEAD), true, placed(clock()), hair, deep);
}

void SOUND::VIEWS::marked(
  SHELL::Session &session, const String &map, Placed placed) {
  const Setting &setting = TRANSPORT::held();
  session.print(std::format(
    "minimap marks tags {} loops {} ends {} head {:g}", setting.tags.size(),
    setting.loops.size(), setting.ends.to > setting.ends.from ? "on" : "off",
    placed(clock())));
}
