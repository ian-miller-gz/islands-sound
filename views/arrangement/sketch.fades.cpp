// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <format>

#include <island/gui/seat.hpp>

#include "../../boards.hpp"
#include "../../fade.hpp"
#include "../../timeline.hpp"
#include "../views.curves.hpp"
#include "arrangement.internal.hpp"
#include "sketch.fades.hpp"

namespace {
using namespace SOUND;
using VIEWS::ARRANGEMENT::Box;

constexpr STRING::Hot STEM = "fade";
constexpr STRING::Hot INK = "fadeline";
constexpr STRING::Hot KIND = "stroke";
constexpr STRING::Hot LINE = "fade {} {} shape {} bend {:g}";

Vector<Flag> stood;
Whole born = 0;

auto boxed(Whole row) -> String {
  return GUI::SAC::SEAT::named(VIEWS::ARRANGEMENT::board().c_str(), row);
}

auto named(const String &box, const FADE::End &end) -> String {
  return std::format("{}.{}.{}", box, ::STEM, end.word);
}

auto clip(const Box &box) -> FADE::Clip {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (box.track >= tracks.size()) return {};
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes = tracks[box.track].lanes;
  if (box.lane >= lanes.size()) return {};
  const Vector<FADE::Clip> &clips = lanes[box.lane].clips;
  return box.placement < clips.size() ? clips[box.placement] : FADE::Clip{};
}

void placed(GUI::Handle page, const String &box) {
  for (const FADE::End &end : FADE::ENDS) {
    const String id = ::named(box, end);
    BOARDS::place(page, box.c_str(), ::KIND, id, {}, {});
    GUI::set(page, id.c_str(), GUI::Style{::INK});
    GUI::set(page, id.c_str(), GUI::Depth{VIEWS::ARRANGEMENT::RAISED});
  }
}

void written(
  GUI::Handle page, const String &id, const FADE::End &end,
  const FADE::Clip &span, Float deep) {
  const FADE::Fade &fade = span.*end.fade;
  const Whole frames = std::min(fade.frames, span.frames);
  GUI::set(page, id.c_str(), GUI::Visibility{frames != 0});
  if (frames == 0) return;
  const Float run = VIEWS::ARRANGEMENT::across(frames);
  const Float edge = end.east ? VIEWS::ARRANGEMENT::across(span.frames) : 0.0f;
  const Float weight =
    GUI::GET::border(page, ::INK) / VIEWS::ARRANGEMENT::scaled();
  VIEWS::stroke(
    page, id, {{edge, deep}, end.east ? -run : run, deep, fade.shape, weight});
}

void shaded(GUI::Handle page, Whole row) {
  const Box box = VIEWS::ARRANGEMENT::drawn(row);
  FADE::Clip span = ::clip(VIEWS::ARRANGEMENT::boxes()[row]);
  span.frames = box.frames;
  if (span.in.frames == 0 && span.out.frames == 0 && !::stood[row]) return;
  const String id = ::boxed(row);
  if (!::stood[row]) ::placed(page, id);
  ::stood[row] = true;
  const Float deep = VIEWS::ARRANGEMENT::inlaid(box.band);
  for (const FADE::End &end : FADE::ENDS)
    ::written(page, ::named(id, end), end, span, deep);
}

}  // namespace

void SOUND::VIEWS::ARRANGEMENT::fades() {
  const GUI::Handle page = VIEWS::document();
  if (VIEWS::reborn(::born)) ::stood.clear();
  ::stood.resize(boxes().size(), false);
  for (Whole row = 0; row < ::stood.size(); ++row) ::shaded(page, row);
}

void SOUND::VIEWS::ARRANGEMENT::fades(SHELL::Session &session) {
  const GUI::Handle page = VIEWS::document();
  for (Whole row = 0; row < ::stood.size(); ++row)
    for (const FADE::End &end : FADE::ENDS) {
      const String id = ::named(::boxed(row), end);
      if (!::stood[row] || !GUI::GET::visibility(page, id.c_str())) continue;
      session.print(std::format(
        ::LINE, row, end.word,
        VIEWS::spoken(GUI::GET::shape(page, id.c_str()).run),
        GUI::GET::bend(page, id.c_str())));
    }
}
