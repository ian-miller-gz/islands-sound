// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>

#include "session.store.internal.hpp"

using namespace SOUND::SESSION;

namespace {

constexpr STRING::Hot SPOKEN[] = {
  FIELD::SCHEMED, FIELD::SETTING, FIELD::FROM,   FIELD::TO,    FIELD::LOOPING,
  FIELD::TEMPO,   FIELD::TRACKS,  FIELD::STOCKS, FIELD::CLIPS, FIELD::RECENTS,
  FIELD::VALUES,  FIELD::HISTORY, FIELD::UNDONE, FIELD::WIRING};

}  // namespace

auto SOUND::SESSION::written(const Session &document) -> FIELDS::Map {
  const SOUND::History &history = document.history;
  const Whole kept =
    std::min<Whole>(HISTORY::HORIZON, Whole(history.edits.size()));
  FIELDS::Map row = unread();
  row[FIELD::SCHEMED] = SCHEMA;
  row[FIELD::SETTING] = spliced(written(document.setting), FIELD::SETTING);
  row[FIELD::TRACKS] = spliced(written(document.arrangement), FIELD::TRACKS);
  row[FIELD::STOCKS] = spliced(written(document.inventory), FIELD::STOCKS);
  row[FIELD::RECENTS] = spliced(written(document.recents), FIELD::RECENTS);
  row[FIELD::WIRING] = spliced(written(document.graph), FIELD::WIRING);
  row[FIELD::VALUES] = spliced(written(document.values), FIELD::VALUES);
  row[FIELD::HISTORY] =
    spliced(written(history, HISTORY::HORIZON), FIELD::HISTORY);
  row[FIELD::UNDONE] = std::min<Whole>(history.undone, kept);
  return row;
}

auto SOUND::SESSION::unspoken(const FIELDS::Map &row) -> FIELDS::Map {
  FIELDS::Map held;
  for (const auto &[field, value] : row) {
    Flag spoken = false;
    for (STRING::Hot known : ::SPOKEN) spoken = spoken || field == known;
    if (!spoken) held[field] = value;
  }
  return held;
}
