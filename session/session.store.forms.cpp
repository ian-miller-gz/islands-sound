// SPDX-License-Identifier: AGPL-3.0-or-later
#include "session.doors.hpp"
#include "session.store.internal.hpp"

using namespace SOUND::SESSION;

namespace {

auto found(const FIELDS::Map &row, STRING::Hot field) -> const FIELDS::Value * {
  const auto at = row.find(field);
  return at == row.end() ? nullptr : &at->second;
}

void read(const FIELDS::Map &row, STRING::Hot field, Whole &value) {
  if (const FIELDS::Value *held = ::found(row, field))
    if (const auto *number = std::get_if<Whole>(held)) value = *number;
}

void read(const FIELDS::Map &row, STRING::Hot field, Float &value) {
  if (const FIELDS::Value *held = ::found(row, field))
    if (const auto *number = std::get_if<Float>(held)) value = *number;
}

void read(const FIELDS::Map &row, STRING::Hot field, Flag &value) {
  if (const FIELDS::Value *held = ::found(row, field))
    if (const auto *flag = std::get_if<Flag>(held)) value = *flag;
}

void read(const FIELDS::Map &row, STRING::Hot field, String &value) {
  if (const FIELDS::Value *held = ::found(row, field))
    if (const auto *text = std::get_if<String>(held)) value = *text;
}

void restored(const FIELDS::Map &row, SOUND::Setting &setting) {
  ::read(row, FIELD::TEMPO, setting.tempos.front().tempo);
}

void stamped(STRING::Hot half, Vector<SOUND::Husk> &husks, Whole from) {
  for (Whole at = from; at < husks.size(); ++at) husks[at].half = half;
}

template <typename Noun>
void half(
  const FIELDS::Map &row, STRING::Hot field, Noun &noun,
  Vector<SOUND::Husk> &husks) {
  String text;
  ::read(row, field, text);
  const Whole from = husks.size();
  SOUND::SESSION::taken(text, noun, husks);
  ::stamped(field, husks, from);
}

template <typename Noun>
void former(
  const FIELDS::Map &row, STRING::Hot field, Noun &noun,
  const SOUND::Graph &wiring, Vector<SOUND::Husk> &husks) {
  String text;
  ::read(row, field, text);
  const Whole from = husks.size();
  SOUND::SESSION::older(text, noun, wiring, husks);
  ::stamped(field, husks, from);
}

void current(
  const FIELDS::Map &row, Whole schema, SOUND::Session &document,
  Vector<SOUND::Husk> &husks) {
  ::half(row, FIELD::SETTING, document.setting, husks);
  ::restored(row, document.setting);
  ::half(row, FIELD::TRACKS, document.arrangement, husks);
  ::half(
    row, schema < STOCKED ? FIELD::CLIPS : FIELD::STOCKS, document.inventory,
    husks);
  ::half(row, FIELD::RECENTS, document.recents, husks);
  ::half(row, FIELD::WIRING, document.graph, husks);
  ::half(row, FIELD::VALUES, document.values, husks);
  ::half(row, FIELD::HISTORY, document.history, husks);
  ::read(row, FIELD::UNDONE, document.history.undone);
}

void migrated(
  const FIELDS::Map &row, Whole schema, SOUND::Session &document,
  Vector<SOUND::Husk> &husks) {
  String text;
  ::read(row, FIELD::WIRING, text);
  const Whole from = husks.size();
  SOUND::SESSION::older(text, document.graph, husks);
  ::stamped(FIELD::WIRING, husks, from);
  ::former(row, FIELD::TRACKS, document.arrangement, document.graph, husks);
  ::former(row, FIELD::CLIPS, document.inventory, document.graph, husks);
  ::half(row, FIELD::RECENTS, document.recents, husks);
  ::restored(row, document.setting);
  SOUND::SESSION::relaid(schema, document.arrangement, document.graph);
}

}  // namespace

auto SOUND::SESSION::schemed(const FIELDS::Map &row) -> Whole {
  Whole schema = 0;
  ::read(row, FIELD::SCHEMED, schema);
  return schema;
}

void SOUND::SESSION::taken(
  const FIELDS::Map &row, Whole schema, Session &document,
  Vector<Husk> &husks) {
  if (schema < NAMED)
    ::migrated(row, schema, document, husks);
  else
    ::current(row, schema, document, husks);
  if (schema >= KINDED && schema < OPTED)
    SOUND::SESSION::culled(document.arrangement, document.graph);
  if (schema < MASTERED)
    SOUND::SESSION::mastered(document.arrangement, document.graph);
  SOUND::SESSION::admitted(schema, document.arrangement, document.graph);
}
