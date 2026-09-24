// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../../commands.hpp"
#include "../../timeline.hpp"
#include "tracks.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot PANEL = "tracks.edit";
constexpr STRING::Hot NAMED = "tracks.edit.name";
constexpr STRING::Hot MODE = "tracks.edit.mode";
constexpr STRING::Hot DRAWER = "tracks.edit.drawer";
constexpr STRING::Hot ACCEPT = "tracks.edit.accept";
constexpr STRING::Hot CANCEL = "tracks.edit.cancel";
constexpr STRING::Hot WHOSE = "tracks.edit";
constexpr STRING::Hot TRACKED = "Track";
constexpr STRING::Hot BUSSED = "Bus";
constexpr Whole BUS = 1;
constexpr Float SHUT_WIDE = 0.0f;
constexpr Float OPEN_WIDE = 110.0f;
constexpr Float DRAWN = 4.0f;

Whole about = NONE;
Flag bussed = false;
Float drawn = SHUT_WIDE;

auto standing(GUI::Handle page) -> Flag {
  return GUI::GET::visibility(page, ::PANEL);
}

auto held() -> const ARRANGEMENT::Track * {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  return ::about < tracks.size() ? &tracks[::about] : nullptr;
}

void lowered(GUI::Handle page) {
  GUI::set(page, ::PANEL, GUI::Visibility{false});
  ::drawn = ::SHUT_WIDE;
  GUI::set(
    page, ::DRAWER, GUI::Extent{::drawn, GUI::GET::extent(page, ::DRAWER).h});
  GUI::set(page, ::DRAWER, GUI::Visibility{false});
  if (GUI::GET::editing(page) == ::NAMED) GUI::edit(page, "");
  if (VIEWS::MENU::standing()) VIEWS::MENU::lower();
  ::about = NONE;
}

void slid(GUI::Handle page) {
  const GUI::Extent panel = GUI::GET::measured(page, VIEWS::TRACKS::NAME);
  ::drawn = VIEWS::eased(::drawn, ::OPEN_WIDE);
  GUI::set(page, ::DRAWER, GUI::Position{panel.w, ::DRAWN});
  GUI::set(
    page, ::DRAWER, GUI::Extent{::drawn, GUI::GET::extent(page, ::DRAWER).h});
}

void accepted(GUI::Handle page) {
  const ARRANGEMENT::Track *track = ::held();
  if (track == nullptr) return ::lowered(page);
  const String name = GUI::GET::text(page, ::NAMED);
  if (!name.empty() && name != track->name)
    COMMANDS::named(::about, NONE, name);
  if (::bussed != track->bus) COMMANDS::bussed(::about, ::bussed);
  ::lowered(page);
}

auto chosen(GUI::Handle page) -> Flag {
  const Whole row = VIEWS::MENU::taken(::WHOSE);
  if (row != NONE) ::bussed = row == ::BUS;
  if (GUI::GET::clicked(page, ::MODE)) {
    VIEWS::MENU::raise({String(::TRACKED), String(::BUSSED)}, ::WHOSE, ::MODE);
  }
  GUI::set(page, ::MODE, GUI::Text{::bussed ? ::BUSSED : ::TRACKED});
  return row != NONE;
}

}  // namespace

void SOUND::VIEWS::TRACKS::edit(Whole track) {
  const GUI::Handle page = document();
  ::about = track;
  const ARRANGEMENT::Track *held = ::held();
  if (held == nullptr) return ::lowered(page);
  GUI::set(page, ::NAMED, GUI::Text{held->name});
  ::bussed = held->bus;
  GUI::set(page, ::PANEL, GUI::Visibility{true});
  ::drawn = ::SHUT_WIDE;
  GUI::set(page, ::DRAWER, GUI::Clipping{true});
  GUI::set(page, ::DRAWER, GUI::Visibility{true});
  ::slid(page);
  GUI::edit(page, ::NAMED);
  GUI::set(page, ::NAMED, GUI::Caret{held->name.size(), 0});
}

void SOUND::VIEWS::TRACKS::editing() {
  const GUI::Handle page = document();
  if (!::standing(page)) return;
  if (::held() == nullptr) return ::lowered(page);
  ::slid(page);
  if (::chosen(page)) return;
  if (GUI::GET::clicked(page, ::ACCEPT)) return ::accepted(page);
  if (GUI::GET::clicked(page, ::CANCEL)) return ::lowered(page);
  if (VIEWS::MENU::standing()) return;
  if (VIEWS::elsewhere(ROWS, ::PANEL)) ::lowered(page);
}

void SOUND::VIEWS::TRACKS::editing(SHELL::Session &session) {
  const GUI::Handle page = document();
  if (!::standing(page)) return session.print("edit down");
  session.print(std::format(
    "edit track {} name {} {}", ::about, String(GUI::GET::text(page, ::NAMED)),
    ::bussed ? "bus" : "track"));
}
