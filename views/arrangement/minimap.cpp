// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../timeline.hpp"
#include "../views.internal.hpp"
#include "arrangement.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot GROUND = "ink";
constexpr STRING::Hot TRACED = "trace";
constexpr Float LEDGE = 3.0f;
constexpr Float HAIRLINE = 2.0f;
constexpr Float SHARE = 0.7f;
constexpr Float HALF = 0.5f;
constexpr Float ORIGIN = 0.0f;

Whole stood = 0;
Whole born = 0;

auto ground() -> String { return VIEWS::ARRANGEMENT::map() + "." + ::GROUND; }

struct Trace {
  Whole track = 0;
  Float at = 0.0f, run = 0.0f;
};

auto traces() -> Vector<::Trace> {
  Vector<::Trace> lines;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  for (Whole track = 0; track < tracks.size(); ++track)
    for (const auto &lane : tracks[track].lanes)
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips)
        lines.push_back(
          {track, VIEWS::ARRANGEMENT::across(span.at),
           VIEWS::ARRANGEMENT::across(span.frames)});
  return lines;
}

auto reach() -> Float {
  Whole end = VIEWS::ARRANGEMENT::bar();
  for (const ARRANGEMENT::Track &track : TIMELINE::held().tracks)
    for (const auto &lane : track.lanes)
      for (const ARRANGEMENT::TRACK::LANE::Clip &span : lane.clips)
        end = std::max(end, span.at + span.frames);
  return VIEWS::ARRANGEMENT::across(end);
}

void build(GUI::Handle page, Whole count) {
  const String bed = ::ground();
  if (::stood == 0 && count != 0)
    BOARDS::place(
      page, VIEWS::ARRANGEMENT::map().c_str(), "panel", bed, {}, {});
  BOARDS::sweep(page, bed.c_str(), count, ::stood);
  for (Whole line = ::stood; line < count; ++line) {
    const String id = GUI::SAC::SEAT::named(bed.c_str(), line);
    BOARDS::place(page, bed.c_str(), "panel", id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::TRACED});
    GUI::NGA::set(page, id.c_str(), GUI::NGA::Pinned{GUI::NGA::Pinned::DOWN});
  }
  ::stood = count;
}

void derive(GUI::Handle page, const String &id, Float wide) {
  const Float field = VIEWS::ARRANGEMENT::scaled();
  const Float scale = field * VIEWS::ARRANGEMENT::RATIO;
  if (scale <= ::ORIGIN || wide <= ::ORIGIN) return;
  GUI::NGA::set(page, id.c_str(), GUI::NGA::Zoom{scale});
  if (!GUI::NGA::GET::stroked(page, id.c_str()).board.empty()) return;
  const String board = VIEWS::ARRANGEMENT::board();
  const Float centre = GUI::NGA::GET::pan(page, board.c_str()).x +
                       VIEWS::ARRANGEMENT::window() * ::HALF;
  const Float west = std::max(
    -VIEWS::ARRANGEMENT::GUTTER / field, centre - wide / scale * ::HALF);
  GUI::NGA::set(page, id.c_str(), GUI::NGA::Pan{west, ::ORIGIN});
}

void strolled(GUI::Handle page, const String &id) {
  const GUI::NGA::Ask stroke = GUI::NGA::GET::stroked(page, id.c_str());
  if (stroke.board.empty()) return;
  VIEWS::wandered();
  const String board = VIEWS::ARRANGEMENT::board();
  const GUI::NGA::Pan held = GUI::NGA::GET::pan(page, board.c_str());
  GUI::NGA::set(
    page, board.c_str(),
    GUI::NGA::Pan{stroke.x - VIEWS::ARRANGEMENT::window() / 2.0f, held.y});
}

}  // namespace

auto SOUND::VIEWS::ARRANGEMENT::stride() -> Float {
  const Float deep =
    GUI::GET::measured(document(), map().c_str()).h - ::LEDGE * 2.0f;
  const Whole tracks = TIMELINE::held().tracks.size();
  if (tracks == 0 || deep <= 0.0f) return ::HAIRLINE;
  return std::max(::HAIRLINE, deep / Float(tracks));
}

void SOUND::VIEWS::ARRANGEMENT::minimap() {
  const GUI::Handle page = document();
  const String id = map();
  const Float wide = GUI::GET::measured(page, id.c_str()).w;
  if (VIEWS::reborn(::born)) ::stood = 0;
  ::strolled(page, id);
  ::derive(page, id, wide);
  const Vector<::Trace> lines = ::traces();
  if (lines.size() != ::stood) ::build(page, lines.size());
  const String bed = ::ground();
  const Float row = stride();
  for (Whole line = 0; line < lines.size(); ++line) {
    const String mark = GUI::SAC::SEAT::named(bed.c_str(), line);
    GUI::set(
      page, mark.c_str(),
      GUI::Position{lines[line].at, ::LEDGE + Float(lines[line].track) * row});
    GUI::set(
      page, mark.c_str(),
      GUI::Extent{lines[line].run, std::max(::HAIRLINE, row * ::SHARE)});
  }
  VIEWS::marked(id, across);
  slider();
}

void SOUND::VIEWS::ARRANGEMENT::minimap(SHELL::Session &session) {
  const GUI::Handle page = document();
  const String id = map();
  session.print(std::format(
    "minimap lines {} reach {:g} ratio {:g}", ::stood, ::reach(), RATIO));
  session.print(std::format(
    "minimap zoom {:g} pan {:g}", GUI::NGA::GET::zoom(page, id.c_str()).value,
    GUI::NGA::GET::pan(page, id.c_str()).x));
  VIEWS::marked(session, id, across);
}
