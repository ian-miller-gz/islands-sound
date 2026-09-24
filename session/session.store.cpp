// SPDX-License-Identifier: AGPL-3.0-or-later
#include <algorithm>
#include <store.hpp>

#include "session.doors.hpp"
#include "session.store.internal.hpp"

using namespace SOUND::SESSION;

namespace {

constexpr STRING::Hot DATABASE = "sound3";
constexpr STRING::Hot TABLE = "sessions";

STORE::Handle database;
Whole arrived = 0;

void forward(const String &name, Whole schema, const SOUND::Session &document) {
  ::arrived = 0;
  if (schema >= SCHEMA) return;
  STORE::put(store(), TABLE, name.c_str(), written(document));
  ::arrived = schema;
}

}  // namespace

auto SOUND::SESSION::store() -> STORE::Handle {
  if (::database.id != 0) return ::database;
  ::database = STORE::open(::DATABASE);
  if (STORE::GET::version(::database) != SCHEMA)
    STORE::SET::version(::database, SCHEMA);
  return ::database;
}

auto SOUND::SESSION::save(
  const String &name, const Setting &setting, const Arrangement &tracks,
  const Inventory &pool, const Graph &wiring, const Vector<Value> &values,
  const History &history) -> Flag {
  if (name.empty()) return false;
  const STORE::Handle opened = store();
  if (opened.id == 0) return false;
  Session document = {
    .name = name,
    .arrangement = tracks,
    .graph = wiring,
    .history = history,
    .inventory = pool,
    .setting = setting,
    .values = values,
    .recents = held().recents};
  STORE::put(opened, TABLE, name.c_str(), written(document));
  adopt(document);
  return true;
}

auto SOUND::SESSION::open(const String &name) -> STRING::Hot {
  FIELDS::Map row;
  if (!STORE::get(store(), TABLE, name.c_str(), row)) return "no such session";
  const Whole schema = schemed(row);
  Session document;
  document.name = name;
  Vector<Husk> husks;
  taken(row, schema, document, husks);
  adopted(husks, unspoken(row));
  adopt(document);
  ::forward(name, schema, document);
  return "";
}

auto SOUND::SESSION::rewritten() -> Whole { return ::arrived; }

auto SOUND::SESSION::names() -> Vector<String> {
  Vector<String> kept;
  for (const STORE::Row &row : STORE::scan(store(), TABLE))
    kept.push_back(row.key);
  std::sort(kept.begin(), kept.end());
  return kept;
}

void SOUND::SESSION::close() {
  STORE::close(database);
  database = {};
}
