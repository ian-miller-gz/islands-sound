// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstdlib>
#include <format>

#include "../commands.hpp"
#include "../control.hpp"
#include "../history.hpp"
#include "../render.hpp"
#include "../timeline.hpp"
#include "../transport.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PLAY = "deck.play";
constexpr STRING::Hot STOP = "deck.stop";
constexpr STRING::Hot BEHIND = "deck.behind";
constexpr STRING::Hot AHEAD = "deck.ahead";
constexpr STRING::Hot RECORD = "deck.record";
constexpr STRING::Hot LOOP = "deck.loop";
constexpr STRING::Hot CHASE = "deck.chase";
constexpr STRING::Hot POSITION = "deck.position";
constexpr STRING::Hot PACE = "deck.tempo";

constexpr Whole SMOOTH = 0;
constexpr Flag BARE = false;

void marked(GUI::Handle page, STRING::Hot door, Flag engaged) {
  GUI::set(page, door, GUI::Style{engaged ? VIEWS::MARK : VIEWS::PLAIN});
}

constexpr STRING::Hot ARMED = "armed";

void reddened(GUI::Handle page, Flag armed) {
  GUI::set(page, ::RECORD, GUI::Style{armed ? ::ARMED : VIEWS::PLAIN});
}

auto taken(GUI::Handle page, Float &tempo) -> Flag {
  if (GUI::GET::dialled(page, ::PACE)) {
    tempo = GUI::GET::value(page, ::PACE);
    return true;
  }
  if (!GUI::GET::committed(page, ::PACE)) return false;
  tempo = std::strtof(GUI::GET::text(page, ::PACE).c_str(), nullptr);
  GUI::edit(page, "");
  return true;
}

void halted() {
  if (TRANSPORT::marker().playing) {
    TRANSPORT::stop();
    RENDER::settle();
    return;
  }
  TRANSPORT::locate(TRANSPORT::mark());
}

void shown(GUI::Handle page, const TRANSPORT::Marker &marker) {
  ::marked(page, ::PLAY, marker.playing);
  ::marked(page, ::LOOP, TRANSPORT::held().looping);
  ::marked(page, ::CHASE, VIEWS::chasing());
  ::reddened(page, CONTROL::reel().armed);
  GUI::set(
    page, ::POSITION,
    GUI::Text{std::format("{:.3f} s", TRANSPORT::seconds(marker.position))});
  GUI::set(
    page, ::PACE,
    GUI::Dial{TRANSPORT::SLOWEST, TRANSPORT::FASTEST, TEMPO, ::SMOOTH, ::BARE});
  if (GUI::GET::editing(page) == ::PACE) return;
  const Float held = TRANSPORT::paced();
  GUI::set(page, ::PACE, GUI::Text{std::format("{:.1f} BPM", held)});
  GUI::set(page, ::PACE, GUI::Value{held});
}

}  // namespace

namespace {

constexpr STRING::Hot TRACKS = "tracks";

Flag dialling = false;

void turning(Flag moving) {
  if (moving == ::dialling) return;
  if (moving)
    HISTORY::begin();
  else
    HISTORY::end();
  ::dialling = moving;
}

void armed() {
  const Flag raise = !CONTROL::reel().armed;
  const Whole tracks = TIMELINE::held().tracks.size();
  if (raise && tracks > 0 && CONTROL::driven() >= tracks) {
    const Whole chosen = VIEWS::cursor(::TRACKS);
    CONTROL::drive(chosen < tracks ? chosen : 0);
  }
  CONTROL::arm(raise);
}

}  // namespace

void SOUND::VIEWS::deck() {
  const GUI::Handle page = sheet();
  if (GUI::GET::clicked(page, ::PLAY)) {
    RENDER::spool();
    TRANSPORT::play();
  }
  if (GUI::GET::clicked(page, ::STOP)) ::halted();
  if (GUI::GET::clicked(page, ::BEHIND)) TRANSPORT::behind();
  if (GUI::GET::clicked(page, ::AHEAD)) TRANSPORT::ahead();
  if (GUI::GET::clicked(page, ::RECORD)) ::armed();
  if (GUI::GET::clicked(page, ::LOOP)) cycled();
  if (GUI::GET::clicked(page, ::CHASE)) VIEWS::chasing(!VIEWS::chasing());
  ::turning(GUI::GET::dialled(page, ::PACE));
  Float tempo = TRANSPORT::paced();
  if (::taken(page, tempo)) COMMANDS::paced(0, tempo);
  ::shown(page, TRANSPORT::marker());
}
