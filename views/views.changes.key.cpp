// SPDX-License-Identifier: AGPL-3.0-or-later
#include <format>

#include "../history.hpp"
#include "../score.hpp"
#include "../transport.hpp"
#include "views.changes.hpp"

namespace {
using namespace SOUND;

constexpr STRING::Hot RESTING = "off";
constexpr STRING::Hot STEMS[] = {".metre.", ".key."};
constexpr Float NOWHERE = 0.0f;
constexpr char SPACE = ' ';

String seat;

auto named(const String &scale) -> Flag {
  for (STRING::Hot row : SCORE::scales())
    if (scale == row) return true;
  return false;
}

auto split(const String &run, Key &row) -> Flag {
  const auto space = run.find(::SPACE);
  if (space == String::npos) return false;
  const Whole tonic = SCORE::classed(run.substr(0, space).c_str());
  const String scale = run.substr(space + 1);
  if (tonic == NONE || !::named(scale)) return false;
  row.tonic = tonic;
  row.scale = scale;
  return true;
}

auto typed(const String &strip, const String &editing) -> Flag {
  for (STRING::Hot stem : ::STEMS)
    if (editing.starts_with(strip + stem)) return true;
  return false;
}

}  // namespace

auto SOUND::VIEWS::CHANGES::beside(const Key &row) -> Float {
  for (const Metre &metre : TRANSPORT::held().metres)
    if (metre.at == row.at) return TYPED + INSET;
  return ::NOWHERE;
}

auto SOUND::VIEWS::CHANGES::worded(const Key &row) -> String {
  if (SCORE::degree(row, row.tonic) == NONE) return String(::RESTING);
  return std::format("{} {}", SCORE::classed(row.tonic), row.scale);
}

void SOUND::VIEWS::CHANGES::dressed(
  GUI::Handle, const String &, const Key &) {}

auto SOUND::VIEWS::CHANGES::taken(
  GUI::Handle page, const String &id, const Key &row) -> Flag {
  if (!GUI::GET::committed(page, id.c_str())) return false;
  Key typed = row;
  if (::split(GUI::GET::text(page, id.c_str()), typed))
    COMMANDS::keyed(row.at, typed.tonic, typed.scale);
  GUI::edit(page, "");
  return true;
}

auto SOUND::VIEWS::CHANGES::keyed(Whole from, Whole to) -> Flag {
  const Key held = TRANSPORT::keyed(from);
  HISTORY::begin();
  const Flag carried =
    COMMANDS::keyed(to, held.tonic, held.scale) && COMMANDS::keyed(from);
  HISTORY::end();
  return carried;
}

void SOUND::VIEWS::CHANGES::seated(GUI::Handle page, const String &strip) {
  const String editing = GUI::GET::editing(page);
  if (editing != ::seat && ::typed(strip, editing))
    GUI::set(
      page, editing.c_str(),
      GUI::Caret{Whole(GUI::GET::text(page, editing.c_str()).size()), 0});
  ::seat = editing;
}
