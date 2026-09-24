// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include <island/input.hpp>

#include "../commands.hpp"
#include "../timeline.hpp"
#include "views.internal.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot DOOR = "export";
constexpr STRING::Hot ASK = "export.ask";
constexpr STRING::Hot ROWS = "export.rows";
constexpr STRING::Hot NOTE = "export.note";
constexpr STRING::Hot TAKE = "export.take";
constexpr STRING::Hot CANCEL = "export.cancel";
constexpr STRING::Hot TICK = "tick";
constexpr STRING::Hot NAMED = "name";
constexpr STRING::Hot CHOSEN = "✓";
constexpr STRING::Hot LIT = "exportchosen";
constexpr STRING::Hot PLAIN = "ink";
constexpr STRING::Hot BUSSED = " (bus)";
constexpr Float HEAD = 32.0f;
constexpr Float FOOT = 64.0f;

Vector<Flag> chosen;

auto step(GUI::Handle page) -> Float {
  return GUI::GET::pitch(page, ::ROWS) + GUI::GET::pad(page, ::ROWS);
}

void rowed(GUI::Handle page, const String &cell, Whole track) {
  const ARRANGEMENT::Track &row = TIMELINE::held().tracks[track];
  const Flag lit = ::chosen[track];
  const String tick = cell + "." + ::TICK, name = cell + "." + ::NAMED;
  GUI::set(page, tick.c_str(), GUI::Text{lit ? String(::CHOSEN) : String()});
  GUI::set(
    page, name.c_str(),
    GUI::Text{row.name + (row.bus ? String(::BUSSED) : String())});
  GUI::set(page, tick.c_str(), GUI::Style{lit ? ::LIT : ::PLAIN});
  GUI::set(page, name.c_str(), GUI::Style{lit ? ::LIT : ::PLAIN});
}

void listed(GUI::Handle page) {
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  ::chosen.resize(tracks.size(), false);
  GUI::set(page, ::ROWS, GUI::Rows{tracks.size()});
  const GUI::Extent size = GUI::GET::extent(page, ::ASK);
  GUI::set(
    page, ::ASK,
    GUI::Extent{size.w, ::HEAD + Float(tracks.size()) * ::step(page) + ::FOOT});
  const Whole first = GUI::GET::first(page, ::ROWS);
  for (Whole row = 0; first + row < tracks.size(); ++row) {
    const String cell = std::format("{}.{}", ::ROWS, row);
    if (!GUI::GET::visibility(page, cell.c_str())) break;
    ::rowed(page, cell, first + row);
  }
}

void raised(GUI::Handle page, Flag on) {
  GUI::set(page, ::ASK, GUI::Visibility{on});
  GUI::set(page, ::NOTE, GUI::Text{String()});
  ::chosen.clear();
  if (on) ::listed(page);
}

void toggled(GUI::Handle page) {
  if (!GUI::GET::activated(page, ::ROWS)) return;
  const Whole row = GUI::GET::cursor(page, ::ROWS);
  if (row < ::chosen.size()) ::chosen[row] = !::chosen[row];
}

auto noted(Whole written, const String &last, const String &refusal) -> String {
  if (!refusal.empty()) return std::format("refused: {}", refusal);
  if (written == 0) return "nothing chosen";
  if (written == 1) return std::format("written: {}", last);
  return std::format("written {}: {}", written, last);
}

void taken(GUI::Handle page) {
  if (!GUI::GET::clicked(page, ::TAKE)) return;
  const Vector<ARRANGEMENT::Track> &tracks = TIMELINE::held().tracks;
  Whole written = 0;
  String last, refusal;
  for (Whole track = 0; track < ::chosen.size() && refusal.empty(); ++track) {
    if (!::chosen[track]) continue;
    const COMMANDS::Export made = COMMANDS::exported(track, String());
    if (!made.refusal.empty())
      refusal = std::format("{} {}", tracks[track].name, made.refusal);
    else
      ++written, last = made.path;
  }
  GUI::set(page, ::NOTE, GUI::Text{::noted(written, last, refusal)});
}

}  // namespace

void SOUND::VIEWS::exporting() {
  const GUI::Handle page = document();
  const Flag standing = GUI::GET::visibility(page, ::ASK);
  if (GUI::GET::clicked(page, ::DOOR)) return ::raised(page, !standing);
  if (!standing) return;
  ::toggled(page);
  ::listed(page);
  ::taken(page);
  if (
    GUI::GET::clicked(page, ::CANCEL) ||
    INPUT::GET::pressed(INPUT::KEYS::ESCAPE) || VIEWS::elsewhere(::DOOR, ::ASK))
    ::raised(page, false);
}
