// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/nga.hpp>
#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../timeline.hpp"
#include "piano.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot BED = "seam";
constexpr STRING::Hot GLYPH = "seamcross";
constexpr STRING::Hot CROSS = "✖";
constexpr STRING::Hot LIST = "tracks";
constexpr Float LETTER = 26.0f;
constexpr Float HALF = 0.5f;
constexpr Float STILL = 0.0f;

struct Seam {
  Whole west = NONE, east = NONE;
  Whole at = 0;
  Flag flush = false;
};

Whole stood = 0;
Whole edged = 0;
Whole born = 0;

auto attended() -> Whole {
  const Whole track = VIEWS::cursor(::LIST);
  return track < TIMELINE::held().tracks.size() ? track : NONE;
}

auto bed() -> String { return VIEWS::ROLL::board() + "." + ::BED; }

auto ordered(Whole track, Whole lane) -> Vector<Whole> {
  const auto &clips = TIMELINE::held().tracks[track].lanes[lane].clips;
  Vector<Whole> rows(clips.size());
  for (Whole row = 0; row < rows.size(); ++row) rows[row] = row;
  std::sort(rows.begin(), rows.end(), [&](Whole one, Whole two) {
    return clips[one].at < clips[two].at;
  });
  return rows;
}

auto seamed() -> Vector<::Seam> {
  const Whole track = ::attended();
  const Whole lane = VIEWS::ROLL::laned(track);
  if (lane == NONE) return {};
  const auto &clips = TIMELINE::held().tracks[track].lanes[lane].clips;
  const Vector<Whole> rows = ::ordered(track, lane);
  Vector<::Seam> seams;
  for (Whole next = 1; next < rows.size(); ++next) {
    const auto &west = clips[rows[next - 1]];
    const auto &east = clips[rows[next]];
    const Whole closes = west.at + west.frames;
    const Whole between =
      closes < east.at ? closes + (east.at - closes) / 2 : east.at;
    seams.push_back(
      {rows[next - 1], rows[next], VIEWS::ROLL::opened(between),
       closes >= east.at});
  }
  return seams;
}

auto flushed(const Vector<::Seam> &seams) -> Vector<Whole> {
  Vector<Whole> pulses;
  for (const ::Seam &seam : seams)
    if (seam.flush) pulses.push_back(seam.at);
  return pulses;
}

void crossed(GUI::Handle page, const Vector<::Seam> &seams) {
  const String bed = ::bed();
  VIEWS::ROLL::SEAMS::built(
    page, bed, seams.size(), ::stood, "button", ::GLYPH);
  const Float scale = VIEWS::ROLL::scaled();
  const Float mark = VIEWS::ROLL::SEAMS::MARK;
  const Float wide = scale > ::STILL ? mark / scale : mark;
  for (Whole seam = 0; seam < seams.size(); ++seam) {
    const String id = GUI::SAC::SEAT::named(bed.c_str(), seam);
    GUI::set(page, id.c_str(), GUI::Text{String(::CROSS)});
    GUI::set(page, id.c_str(), GUI::Size{::LETTER});
    GUI::set(
      page, id.c_str(),
      GUI::Position{
        VIEWS::ROLL::across(seams[seam].at) - wide * ::HALF,
        VIEWS::ROLL::DEEP * ::HALF - mark * ::HALF});
    GUI::set(page, id.c_str(), GUI::Extent{wide, mark});
  }
}

auto pressed(GUI::Handle page, const Vector<::Seam> &seams) -> Flag {
  const String bed = ::bed();
  for (Whole seam = 0; seam < seams.size(); ++seam) {
    const String id = GUI::SAC::SEAT::named(bed.c_str(), seam);
    if (!GUI::GET::clicked(page, id.c_str())) continue;
    const Whole track = ::attended();
    VIEWS::joined(
      track, VIEWS::ROLL::laned(track), {seams[seam].west, seams[seam].east});
    return true;
  }
  return false;
}

}  // namespace

void SOUND::VIEWS::ROLL::SHED::seams() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) {
    ::stood = ::edged = 0;
    return;
  }
  SEAMS::bared(page, ::bed(), ::stood);
  SEAMS::bared(page, board() + "." + SEAMS::EDGE, ::edged);
}

void SOUND::VIEWS::ROLL::seams() {
  const GUI::Handle page = document();
  if (VIEWS::reborn(::born)) ::stood = ::edged = 0;
  const Vector<::Seam> seams = ::seamed();
  if (::pressed(page, seams)) return;
  VIEWS::ROLL::SEAMS::lined(page, ::flushed(seams), ::edged);
  ::crossed(page, seams);
}

void SOUND::VIEWS::ROLL::seams(SHELL::Session &session) {
  const Vector<::Seam> seams = ::seamed();
  session.print(std::format("seams {}", seams.size()));
  for (Whole seam = 0; seam < seams.size(); ++seam)
    session.print(std::format(
      "seam {} between {} {} at {}", seam, seams[seam].west, seams[seam].east,
      seams[seam].at));
}
