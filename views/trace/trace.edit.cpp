// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../commands.hpp"
#include "../../graph.hpp"
#include "../../kind.hpp"
#include "../../timeline.hpp"
#include "trace.internal.hpp"

namespace {
using namespace SOUND;
namespace TRACE = SOUND::VIEWS::TRACE;

constexpr STRING::Hot PANEL = "trace.edit";
constexpr STRING::Hot NAMED = "trace.edit.name";
constexpr STRING::Hot KINDED = "trace.edit.kind";
constexpr STRING::Hot ADMIT = "trace.edit.admit";
constexpr STRING::Hot NOTE = "trace.edit.note";
constexpr STRING::Hot DRAWER = "trace.edit.drawer";
constexpr STRING::Hot ACCEPT = "trace.edit.accept";
constexpr STRING::Hot CANCEL = "trace.edit.cancel";
constexpr STRING::Hot ADMITS = "input on";
constexpr STRING::Hot SHUT = "input off";
constexpr STRING::Hot LIT = "loose";
constexpr STRING::Hot PLAIN = "place";
constexpr STRING::Hot LADEN = "the lane holds clips: its kind stays";
constexpr STRING::Hot KINDS = "trace.edit.kind";
constexpr Float SHUT_WIDE = 0.0f;
constexpr Float OPEN_WIDE = 110.0f;
constexpr Float DRAWN = 4.0f;

struct About {
  Whole track = NONE, lane = NONE;
};

About about;
Flag admitting = false;
Whole kind = KIND::AUDIO;
Float drawn = SHUT_WIDE;

auto standing(GUI::Handle page) -> Flag {
  return GUI::GET::visibility(page, ::PANEL);
}

auto held() -> const ARRANGEMENT::TRACK::Lane * {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  if (::about.track >= tracks.size()) return nullptr;
  const Vector<ARRANGEMENT::TRACK::Lane> &lanes = tracks[::about.track].lanes;
  return ::about.lane < lanes.size() ? &lanes[::about.lane] : nullptr;
}

auto admits() -> Flag {
  const String root = TIMELINE::held().tracks[::about.track].root;
  return GRAPH::rooted(root) && GRAPH::admitted(root, ::about.lane) != NONE;
}

void lowered(GUI::Handle page) {
  GUI::set(page, ::PANEL, GUI::Visibility{false});
  if (VIEWS::MENU::standing()) VIEWS::MENU::lower();
  ::drawn = ::SHUT_WIDE;
  GUI::set(
    page, ::DRAWER, GUI::Extent{::drawn, GUI::GET::extent(page, ::DRAWER).h});
  GUI::set(page, ::DRAWER, GUI::Visibility{false});
  if (GUI::GET::editing(page) == ::NAMED) GUI::edit(page, "");
  ::about = {};
}

void slid(GUI::Handle page) {
  const GUI::Extent panel = GUI::GET::measured(page, TRACE::NAME);
  ::drawn = VIEWS::eased(::drawn, ::OPEN_WIDE);
  GUI::set(page, ::DRAWER, GUI::Position{panel.w, ::DRAWN});
  GUI::set(
    page, ::DRAWER, GUI::Extent{::drawn, GUI::GET::extent(page, ::DRAWER).h});
}

void accepted(GUI::Handle page) {
  const ARRANGEMENT::TRACK::Lane *lane = ::held();
  if (lane == nullptr) return ::lowered(page);
  const String name = GUI::GET::text(page, ::NAMED);
  if (!name.empty() && name != lane->name)
    COMMANDS::named(::about.track, ::about.lane, name);
  if (
    ::kind != lane->kind &&
    !COMMANDS::recast(::about.track, ::about.lane, ::kind)) {
    GUI::set(page, ::NOTE, GUI::Text{::LADEN});
    return;
  }
  if (::admitting != ::admits())
    COMMANDS::admitted(::about.track, ::about.lane, ::admitting);
  ::lowered(page);
}

auto worded(GUI::Handle page) -> Flag {
  const Flag took = TRACE::kinded(page, ::KINDED, ::KINDS, ::kind);
  if (GUI::GET::clicked(page, ::ADMIT)) ::admitting = !::admitting;
  GUI::set(page, ::ADMIT, GUI::Text{::admitting ? ::ADMITS : ::SHUT});
  GUI::set(page, ::ADMIT, GUI::Style{::admitting ? ::LIT : ::PLAIN});
  return took;
}

}  // namespace

void SOUND::VIEWS::TRACE::edit(Whole track, Whole lane) {
  const GUI::Handle page = document();
  ::about = {track, lane};
  const ARRANGEMENT::TRACK::Lane *held = ::held();
  if (held == nullptr) return ::lowered(page);
  GUI::set(page, ::NAMED, GUI::Text{held->name});
  ::kind = held->kind;
  ::admitting = ::admits();
  GUI::set(page, ::NOTE, GUI::Text{""});
  GUI::set(page, ::PANEL, GUI::Visibility{true});
  ::drawn = ::SHUT_WIDE;
  GUI::set(page, ::DRAWER, GUI::Clipping{true});
  GUI::set(page, ::DRAWER, GUI::Visibility{true});
  ::slid(page);
  GUI::edit(page, ::NAMED);
  GUI::set(page, ::NAMED, GUI::Caret{held->name.size(), 0});
}

void SOUND::VIEWS::TRACE::editing() {
  const GUI::Handle page = document();
  if (!::standing(page)) return;
  if (::held() == nullptr) return ::lowered(page);
  ::slid(page);
  if (::worded(page)) return;
  if (GUI::GET::clicked(page, ::ACCEPT)) return ::accepted(page);
  if (GUI::GET::clicked(page, ::CANCEL)) return ::lowered(page);
  if (VIEWS::MENU::standing()) return;
  if (VIEWS::elsewhere(LANES, ::PANEL)) ::lowered(page);
}

void SOUND::VIEWS::TRACE::editing(SHELL::Session &session) {
  const GUI::Handle page = document();
  if (!::standing(page)) return session.print("edit down");
  session.print(std::format(
    "edit track {} lane {} name {} kind {} {}", ::about.track, ::about.lane,
    String(GUI::GET::text(page, ::NAMED)), KIND::spoken(::kind),
    ::admitting ? ::ADMITS : ::SHUT));
}
